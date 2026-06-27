//
// Created by janwin443 on 6/19/26.
//

#ifndef OFL_CPP_LEXER_H
#define OFL_CPP_LEXER_H

#endif //OFL_CPP_LEXER_H

#pragma once
#include <string>
#include <vector>
#include <functional>
#include "token.h"

class Lexer {
public:
    explicit Lexer(std::string source);
    std::vector<Token> tokenize();

private:
    std::string source_;
    size_t pos_ = 0;
    int line_ = 1;
    std::vector<Token> tokens_{};
    std::vector<int> indent_stack_ = {0};
    std::string read_while(const std::function<bool(char)>& condition);
    void next_token();
    void handle_indent();
    void read_number();
};