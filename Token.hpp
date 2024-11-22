#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <unordered_map>
#include <string>
#include <iostream>

class Token
{
public:
    enum Type
    {
        ERROR,
        ADD,
        SUB,
        MUL,
        DIV,
        NUM,
        END,
        ASSIGN,
        IF,
        ELSE,
        COMMA,
        FOR,
        RETURN,

        GREATER_THAN,  // >
        GREATER_EQUAL, // >=
        EQUAL,         // ==

        FUN,    // fun
        ENDFUN, // endfun

        LEFT_PARENTHESIS,  // (
        RIGHT_PARENTHESIS, // )

        VAL, // val
        VAR, // var
        ID,  // id

        COLON,         //:
        PRINTLN,       // println
        LEFT_BRACKET,  // {
        RIGHT_BRACKET, // }

        LINE_BREAK, // \n
        SEMICOLON,  // ;
        DOUBLE_DOT, // ..
        IN,         // in
    };

private:
    Type _type;
    std::string _text;
    static const std::unordered_map<Type, std::string> typeToStringMap;

public:
    Token(Type type);
    Token(Type type, char c);
    Token(Type type, std::string s);
    Token(Type type, const std::string &source, int first, int last);
    auto getType() const -> Type;
    auto getText() const -> std::string;
    static std::string typeToString(Type type);
    friend std::ostream &operator<<(std::ostream &outs, const Token &tok);
    friend std::ostream &operator<<(std::ostream &outs, const Token *tok);
};

#endif // TOKEN_HPP