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
    outs << "Token(" << Token::typeToString(tok._type) << ')';
    return outs;
}

std::ostream &operator<<(std::ostream &outs, const Token *tok)
{
    outs << "Token(" << Token::typeToString(tok->_type) << ')';
    return outs;
}

const std::unordered_map<Token::Type, std::string> Token::typeToStringMap = {
    {ERROR, "ERROR"},
    {ADD, "ADD"},
    {SUB, "SUB"},
    {MUL, "MUL"},
    {DIV, "DIV"},
    {NUM, "NUM"},
    {END, "END"},
    {ASSIGN, "ASSIGN"},
    {IF, "IF"},
    {ELSE, "ELSE"},
    {COMMA, "COMMA"},
    {FOR, "FOR"},
    {RETURN, "RETURN"},
    {GREATER_THAN, "GREATER_THAN"},
    {GREATER_EQUAL, "GREATER_EQUAL"},
    {EQUAL, "EQUAL"},
    {FUN, "FUN"},
    {ENDFUN, "ENDFUN"},
    {LEFT_PARENTHESIS, "LEFT_PARENTHESIS"},
    {RIGHT_PARENTHESIS, "RIGHT_PARENTHESIS"},
    {VAL, "VAL"},
    {VAR, "VAR"},
    {ID, "ID"},
    {COLON, "COLON"},
    {PRINTLN, "PRINTLN"},
    {LEFT_BRACKET, "LEFT_BRACKET"},
    {RIGHT_BRACKET, "RIGHT_BRACKET"},
    {LINE_BREAK, "LINE_BREAK"},
    {SEMICOLON, "SEMICOLON"},
    {DOUBLE_DOT, "DOUBLE_DOT"},
    {IN, "IN"}};