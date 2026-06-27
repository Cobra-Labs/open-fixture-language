//
// Created by janwin443 on 6/19/26.
//

// src/lexer.cpp
#include "../include/ofl/lexer.h"
#include <cctype>
#include <unordered_map>
#include <unordered_set>
#include <stdexcept>

Lexer::Lexer(std::string source) : source_(std::move(source)) {}

namespace {
    const std::unordered_set<std::string> KEYWORDS = {
        "fixture", "channel", "mode", "cap",
        "attribute", "resolution", "manufacturer",
        "type", "weight", "power", "range",
        "open", "closed", "variable", "8bit", "16bit", "8ch", "16ch"
    };
}

std::vector<Token> Lexer::tokenize() {
    while (pos_ < source_.size()) {
        next_token();
    }
    while (indent_stack_.size() > 1) {
        indent_stack_.pop_back();
        tokens_.push_back({TokenType::Dedent, "", line_});
    }
    tokens_.push_back({TokenType::Eof, "", line_});
    return tokens_;
}

std::string Lexer::read_while(const std::function<bool(char)>& condition) {
    const size_t start = pos_;
    while (pos_ < source_.size() && condition(source_[pos_])) {
        pos_++;
    }
    return source_.substr(start, pos_ - start);
}

void Lexer::next_token() {
    const char c = source_[pos_];

    // Kommentar oder Farbe (#FF0000)
    if (c == '#') {
        static const std::string hex_chars = "#0123456789abcdefABCDEF";
        if (pos_ + 1 < source_.size() && hex_chars.find(source_[pos_ + 1]) != std::string::npos) {
            const std::string color = read_while([](char ch) {
                static const std::string allowed = "#0123456789ABCDEFabcdef";
                return allowed.find(ch) != std::string::npos;
            });
            tokens_.push_back({TokenType::Color, color, line_});
        } else {
            read_while([](char ch) { return ch != '\n'; }); // Kommentar bis Zeilenende verwerfen
        }
        return;
    }

    // Zeilenumbruch
    if (c == '\n') {
        tokens_.push_back({TokenType::Newline, "\n", line_});
        line_++;
        pos_++;
        handle_indent();
        return;
    }

    // Leerzeichen überspringen
    if (c == ' ' || c == '\t') {
        pos_++;
        return;
    }

    // @ Include
    if (c == '@') {
        pos_++;
        const std::string path = read_while([](char ch) {
            return ch != ' ' && ch != '\n' && ch != '\t';
        });
        tokens_.push_back({TokenType::At, path, line_});
        return;
    }

    // String "..."
    if (c == '"') {
        pos_++;
        const std::string s = read_while([](char ch) { return ch != '"'; });
        pos_++; // schließendes " überspringen
        tokens_.push_back({TokenType::String, s, line_});
        return;
    }

    // Zahl, Range, oder Zahl+Einheit
    if (std::isdigit(static_cast<unsigned char>(c))) {
        read_number();
        return;
    }

    // Keyword oder Identifier
    if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
        const std::string word = read_while([](char ch) {
            return std::isalnum(static_cast<unsigned char>(ch)) || ch == '_';
        });
        if (KEYWORDS.contains(word)) {
            tokens_.push_back({TokenType::Keyword, word, line_});
        } else {
            tokens_.push_back({TokenType::Identifier, word, line_});
        }
        return;
    }

    // Einzelzeichen
    static const std::unordered_map<char, TokenType> single = {
        {':', TokenType::Colon},
        {',', TokenType::Comma},
        {'(', TokenType::LParen},
        {')', TokenType::RParen},
        {'=', TokenType::Equals},
    };
    if (auto it = single.find(c); it != single.end()) {
        tokens_.push_back({it->second, std::string(1, c), line_});
        pos_++;
        return;
    }

    // Unbekannt
    throw std::runtime_error(
        "Unbekanntes Zeichen '" + std::string(1, c) + "' in Zeile " + std::to_string(line_));
}

void Lexer::handle_indent() {
    int spaces = 0;
    while (pos_ < source_.size()) {
        if (source_[pos_] == ' ') {
            spaces++;
            pos_++;
        } else if (source_[pos_] == '\t') {
            spaces += 4;
            pos_++;
        } else {
            break;
        }
    }

    if (pos_ < source_.size() && source_[pos_] == '\n') {
        return; // leere Zeile ignorieren
    }

    const int current = indent_stack_.back();
    if (spaces > current) {
        indent_stack_.push_back(spaces);
        tokens_.push_back({TokenType::Indent, "", line_});
    } else if (spaces < current) {
        while (indent_stack_.back() > spaces) {
            indent_stack_.pop_back();
            tokens_.push_back({TokenType::Dedent, "", line_});
        }
    }
}

void Lexer::read_number() {
    std::string num = read_while([](char c) {
        return std::isdigit(static_cast<unsigned char>(c));
    });

    // Range? 0..9
    if (pos_ + 1 < source_.size() && source_[pos_] == '.' && source_[pos_ + 1] == '.') {
        pos_ += 2;
        const std::string num2 = read_while([](char c) {
            return std::isdigit(static_cast<unsigned char>(c));
        });
        tokens_.push_back({TokenType::Range, num + ".." + num2, line_});
        return;
    }
    // Dezimalzahl? 19.5
    else if (pos_ < source_.size() && source_[pos_] == '.') {
        pos_++;
        const std::string num3 = read_while([](char c) {
            return std::isdigit(static_cast<unsigned char>(c));
        });
        num = num + "." + num3;
    }

    // Einheit direkt dahinter? 540deg, 480W, 19.5kg
    const std::string unit = read_while([](char c) {
        return std::isalpha(static_cast<unsigned char>(c));
    });

    if (!unit.empty()) {
        if (const std::string combined = num + unit; KEYWORDS.contains(combined) || unit == "ch") {
            tokens_.push_back({TokenType::Keyword, combined, line_});
            return;
        }
        tokens_.push_back({TokenType::Number, num, line_});
        tokens_.push_back({TokenType::Unit, unit, line_});
        return;
    }

    tokens_.push_back({TokenType::Number, num, line_});
}

