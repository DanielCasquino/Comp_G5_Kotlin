#include "token.hpp"
#include <iostream>

Token::Token(Type type) : _type(type), _text("") {}

Token::Token(Type type, char c) : _type(type), _text(1, c) {}

Token::Token(Type type, std::string s) : _type(type), _text(std::move(s)) {}

Token::Token(Type type, const std::string &source, int first, int last)
    : _type(type), _text(source.substr(first, last - first + 1)) {}

auto Token::getType() const -> Type
{
    return _type;
}

auto Token::getText() const -> std::string
{
    return _text;
}

std::string Token::typeToString(Type type)
{
    return typeToStringMap.at(type);
}

std::ostream &operator<<(std::ostream &outs, const Token &tok)
{
    outs << "Token(" << Token::typeToString(tok._type) << ", \"" << tok._text << "\")";
    return outs;
}

std::ostream &operator<<(std::ostream &outs, const Token *tok)
{
    outs << "Token(" << Token::typeToString(tok->_type) << ", \"" << tok->_text << "\")";
    return outs;
}

const std::unordered_map<Token::Type, std::string> Token::typeToStringMap = {
    {ERROR, "ERROR"},
    {ADD, "ADD"},
    {SUB, "SUB"},
    {MUL, "MUL"},
    {DIV, "DIV"},
    {LEFT_PARENTHESIS, "LEFT_PARENTHESIS"},
    {RIGHT_PARENTHESIS, "RIGHT_PARENTHESIS"},
    {FUN, "FUN"},
    {LEFT_BRACKETS, "LEFT_BRACKETS"},
    {RIGHT_BRACKETS, "RIGHT_BRACKETS"},
    {ASSIGN, "ASSIGN"},
    {COLON, "COLON"},
    {INT, "INT"},
    {VAR, "VAR"},
    {COMMENT, "COMMENT"},
    {COMMENT_START, "COMMENT_START"},
    {COMMENT_END, "COMMENT_END"},
    {PRINTLN, "PRINTLN"},
    {COMMA, "COMMA"},
    {RETURN, "RETURN"},
    {VAL, "VAL"},
    {POINT, "POINT"},
    {IF, "IF"},
    {ELSE, "ELSE"},
    {GREATER_THAN, "GREATER_THAN"},
    {LESS_THAN, "LESS_THAN"},
    {GREATER_EQUAL, "GREATER_EQUAL"},
    {LESS_EQUAL, "LESS_EQUAL"},
    {EQUAL, "EQUAL"}};