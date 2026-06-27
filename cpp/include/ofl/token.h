//
// Created by janwin443 on 6/19/26.
//

#ifndef OFL_CPP_TOKEN_H
#define OFL_CPP_TOKEN_H

#endif //OFL_CPP_TOKEN_H

// include/ofl/token.h
#pragma once
#include <string>

enum class TokenType {
    // Struktur
    Keyword,
    Identifier,
    Colon,
    Newline,
    Indent,
    Dedent,
    Eof,

    // Werte
    Number,
    String,
    Color,
    Range,
    Unit,

    // Sonderzeichen
    At,
    Comma,
    LParen,
    RParen,
    Equals
};

struct Token {
    TokenType type;
    std::string value;
    int line;
};

// am Ende von token.h
inline std::string to_string(TokenType type) {
    switch (type) {
        case TokenType::Keyword:    return "KEYWORD";
        case TokenType::Identifier: return "IDENTIFIER";
        case TokenType::Colon:      return "COLON";
        case TokenType::Newline:    return "NEWLINE";
        case TokenType::Indent:     return "INDENT";
        case TokenType::Dedent:     return "DEDENT";
        case TokenType::Eof:        return "EOF";
        case TokenType::Number:     return "NUMBER";
        case TokenType::String:     return "STRING";
        case TokenType::Color:      return "COLOR";
        case TokenType::Range:      return "RANGE";
        case TokenType::Unit:       return "UNIT";
        case TokenType::At:         return "AT";
        case TokenType::Comma:      return "COMMA";
        case TokenType::LParen:     return "LPAREN";
        case TokenType::RParen:     return "RPAREN";
        case TokenType::Equals:     return "EQUALS";
    }
    return "UNKNOWN";
}