//
// Created by janwin443 on 6/27/26.
//

#pragma once
#include <vector>
#include <memory>
#include "token.h"
#include "nodes.h"
#include "error.h"

namespace ofl {

class Parser {
public:
    explicit Parser(std::vector<Token> tokens, std::string filename = "<input>");
    FixtureNode parse();

private:
    std::vector<Token> tokens_;
    size_t pos_ = 0;
    std::string filename_;

    // Token-Navigation
    const Token& current() const;
    const Token& peek(int offset = 1) const;
    Token consume();
    Token expect(TokenType type, const std::string& hint = "");
    bool at(TokenType type) const;
    bool at_end() const;
    void skip_newlines();

    // Parse-Methoden
    FixtureNode  parse_fixture();
    ChannelNode  parse_channel();
    ModeNode     parse_mode();
    IncludeNode  parse_include();
    CapNode      parse_cap();
    std::map<std::string, ParamValue> parse_params();
};

} // namespace ofl