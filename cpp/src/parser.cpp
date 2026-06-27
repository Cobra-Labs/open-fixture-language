//
// Created by janwin443 on 6/27/26.
//

#include "ofl/parser.h"
#include "ofl/nodes.h"

namespace ofl {

    Parser::Parser(std::vector<Token> tokens, std::string filename)
        : tokens_(std::move(tokens))
        , filename_(std::move(filename))
    {}

    const Token& Parser::current() const {
        return tokens_[pos_];
    }

    const Token& Parser::peek(int offset) const {
        const size_t idx = pos_ + offset;
        if (idx >= tokens_.size()) return tokens_.back(); // EOF
        return tokens_[idx];
    }

    Token Parser::consume() {
        Token t = tokens_[pos_];
        if (pos_ < tokens_.size() - 1) pos_++;
        return t;
    }

    Token Parser::expect(TokenType type, const std::string& hint) {
        if (current().type != type) {
            throw OflError(
                ErrorCode::UnexpectedToken,
                "Expected " + to_string(type) + " but got '"
                    + current().value + "' (" + to_string(current().type) + ")",
                hint.empty() ? "Check the syntax of your .ofl file" : hint,
                {filename_, current().line, 0}
            );
        }
        return consume();
    }

    bool Parser::at(TokenType type) const {
        return current().type == type;
    }

    bool Parser::at_end() const {
        return current().type == TokenType::Eof;
    }

    void Parser::skip_newlines() {
        while (at(TokenType::Newline)) consume();
    }

    FixtureNode Parser::parse() {
        skip_newlines();
        return parse_fixture();
    }

    FixtureNode Parser::parse_fixture() {
    const int start_line = current().line;

    // "fixture"
    expect(TokenType::Keyword, "Expected 'fixture' keyword at start of file");

    // Name
    const Token name_tok = expect(TokenType::Identifier, "Expected fixture name after 'fixture'");

    // ":"
    expect(TokenType::Colon, "Expected ':' after fixture name");

    // Newline + Indent
    expect(TokenType::Newline, "Expected newline after fixture declaration");
    expect(TokenType::Indent, "Expected indented block after fixture declaration");

    FixtureNode fixture;
    fixture.name = name_tok.value;
    fixture.line = start_line;

    // Meta-Felder und Children parsen
    skip_newlines();
    while (!at_end() && !at(TokenType::Dedent)) {
        skip_newlines();
        if (at_end() || at(TokenType::Dedent)) break;

        if (at(TokenType::Keyword) && current().value == "channel") {
            fixture.children.push_back(parse_channel());
        } else if (at(TokenType::Keyword) && current().value == "mode") {
            fixture.children.push_back(parse_mode());
        } else if (at(TokenType::At)) {
            fixture.children.push_back(parse_include());
        } else if (at(TokenType::Keyword)) {
            // Meta-Feld: manufacturer, type, weight, power etc.
            const std::string key = consume().value;
            expect(TokenType::Colon, "Expected ':' after meta field '" + key + "'");
            // Wert: alles bis Newline zusammensammeln
            std::string value;
            while (!at(TokenType::Newline) && !at_end()) {
                if (!value.empty()) value += " ";
                value += consume().value;
            }
            fixture.meta[key] = value;
            skip_newlines();
        } else {
            throw OflError(
                ErrorCode::UnexpectedToken,
                "Unexpected token '" + current().value + "' in fixture body",
                "Expected a meta field (manufacturer, type, ...), channel, mode, or @include",
                {filename_, current().line, 0}
            );
        }
    }

        if (at(TokenType::Dedent)) consume();

        return fixture;
    }

    CapNode Parser::parse_cap() {
        const int start_line = current().line;
        consume(); // "cap" keyword

        // Range: 0..255
        const Token range_tok = expect(TokenType::Range,
            "Expected range after 'cap', e.g. 'cap 0..255'");

        // Range aufsplitten: "0..255" -> start=0, end=255
        const std::string& range_str = range_tok.value;
        const size_t dot_pos = range_str.find("..");
        const int range_start = std::stoi(range_str.substr(0, dot_pos));
        const int range_end   = std::stoi(range_str.substr(dot_pos + 2));

        expect(TokenType::Colon, "Expected ':' after cap range");

        // Cap-Typ: "open", "gobo", "color", "strobe" etc.
        std::string cap_type;
        std::map<std::string, ParamValue> params;

        if (at(TokenType::Keyword) || at(TokenType::Identifier)) {
            cap_type = consume().value;
        }

        // Optionale Parameter: gobo(slot=1, spin=cw)
        if (at(TokenType::LParen)) {
            params = parse_params();
        }

        // Newline konsumieren
        if (at(TokenType::Newline)) consume();
        skip_newlines();

        CapNode cap;
        cap.range_start = range_start;
        cap.range_end   = range_end;
        cap.cap_type    = cap_type;
        cap.params      = params;
        cap.line        = start_line;
        return cap;
    }

    ChannelNode Parser::parse_channel() {
        const int start_line = current().line;

        consume(); // "channel" keyword

        const Token name_tok = expect(TokenType::Identifier,
            "Expected channel name after 'channel'");
        expect(TokenType::Colon, "Expected ':' after channel name");
        expect(TokenType::Newline, "Expected newline after channel declaration");
        expect(TokenType::Indent, "Expected indented block after channel declaration");

        ChannelNode channel;
        channel.name = name_tok.value;
        channel.line = start_line;

        skip_newlines();
        while (!at_end() && !at(TokenType::Dedent)) {
            skip_newlines();
            if (at_end() || at(TokenType::Dedent)) break;

            if (at(TokenType::Keyword) && current().value == "cap") {
                channel.caps.push_back(parse_cap());
            } else if (at(TokenType::Keyword)) {
                const std::string key = consume().value;
                expect(TokenType::Colon, "Expected ':' after '" + key + "'");
                std::string value;
                while (!at(TokenType::Newline) && !at_end()) {
                    if (!value.empty()) value += " ";
                    value += consume().value;
                }
                if (key == "attribute")  channel.attribute  = value;
                else if (key == "resolution") channel.resolution = value;
                else channel.extra[key] = value;
                skip_newlines();
            } else {
                throw OflError(
                    ErrorCode::UnexpectedToken,
                    "Unexpected token '" + current().value + "' in channel '" + channel.name + "'",
                    "Expected 'attribute', 'resolution', 'cap', or other channel field",
                    {filename_, current().line, 0}
                );
            }
        }

        if (at(TokenType::Dedent)) consume();
        return channel;
    }

    std::map<std::string, ParamValue> Parser::parse_params() {
        std::map<std::string, ParamValue> params;
        consume(); // "("

        while (!at(TokenType::RParen) && !at_end()) {
            // key
            const Token key_tok = expect(TokenType::Identifier,
                "Expected parameter name");
            expect(TokenType::Equals, "Expected '=' after parameter name");

            // value: Number, String, Identifier, oder Color
            ParamValue value;
            if (at(TokenType::Number)) {
                value = std::stoi(consume().value);
            } else if (at(TokenType::String)) {
                value = consume().value;
            } else if (at(TokenType::Identifier) || at(TokenType::Keyword)) {
                value = consume().value;
            } else if (at(TokenType::Color)) {
                value = consume().value;
            } else {
                throw OflError(
                    ErrorCode::UnexpectedToken,
                    "Unexpected token '" + current().value + "' as parameter value",
                    "Expected a number, string, identifier, or color",
                    {filename_, current().line, 0}
                );
            }
            params[key_tok.value] = value;

            // optionales Komma
            if (at(TokenType::Comma)) consume();
        }

        expect(TokenType::RParen, "Expected ')' to close parameter list");
        return params;
    }

    IncludeNode Parser::parse_include() {
        IncludeNode inc;
        inc.line = current().line;
        inc.raw_path = current().value; // AT-Token hat den Pfad als Value
        consume(); // AT-Token
        skip_newlines();
        return inc;
    }
    
    ModeNode Parser::parse_mode() {
        ModeNode mode;
        mode.line = current().line;
        consume(); // "mode" keyword

        // Name: "standard"
        mode.name = expect(TokenType::Identifier,
            "Expected mode name after 'mode'").value;

        // Optionale Kanalzahl: (16ch)
        if (at(TokenType::LParen)) {
            consume(); // "("
            if (at(TokenType::Keyword)) {
                const std::string ch_str = consume().value; // "16ch"
                mode.channel_count = std::stoi(ch_str.substr(0,
                    ch_str.find("ch")));
            }
            expect(TokenType::RParen, "Expected ')' after channel count");
        }

        expect(TokenType::Colon, "Expected ':' after mode declaration");
        expect(TokenType::Newline, "Expected newline after mode declaration");
        expect(TokenType::Indent, "Expected indented block after mode declaration");

        skip_newlines();
        while (!at_end() && !at(TokenType::Dedent)) {
            skip_newlines();
            if (at_end() || at(TokenType::Dedent)) break;

            // Slot-Liste: "1,2: pan" oder "7: gobo"
            std::vector<int> slots;
            while (at(TokenType::Number)) {
                slots.push_back(std::stoi(consume().value));
                if (at(TokenType::Comma)) consume();
            }
            expect(TokenType::Colon, "Expected ':' after slot number(s)");
            const std::string channel_name = expect(TokenType::Identifier,
                "Expected channel name after ':'").value;

            for (int slot : slots) {
                mode.mappings[slot] = channel_name;
            }

            if (at(TokenType::Newline)) consume();
            skip_newlines();
        }

        if (at(TokenType::Dedent)) consume();
        return mode;
    }

} // namespace ofl
