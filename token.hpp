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
        ERROR, //

        ADD, // +
        SUB, // -
        MUL, // *
        DIV, // /
        NUM,

        LEFT_PARENTHESIS,  // (
        RIGHT_PARENTHESIS, // )

        FUN,    // fun
        ENDFUN, // endfun

        END,
        ASSIGN,
        IF,
        ELSE,
        FOR,
        DO,
        WHILE,
        RETURN,
        PRINTLN,
        COMMA,

        GREATER_THAN,  // >
        LESS_THAN,     // <
        GREATER_EQUAL, // >=
        LESS_EQUAL,    // <=
        EQUAL,         // ==
        NOT_EQUAL,     // !=

        LEFT_BRACKET,  // {
        RIGHT_BRACKET, // }

        VAL,
        VAR, // var
        ID,

        COLON, // :

        LINE_BREAK, // \n
        SEMICOLON,  // ;
        DOUBLE_DOT, // ..
        IN,         // in

        DOWNTO,
        STEP
    };

    Token(Type type);
    Token(Type type, char c);
    Token(Type type, std::string s);
    Token(Type type, const std::string &source, int first, int last);
    auto getType() const -> Type;
    auto getText() const -> std::string;
    static std::string typeToString(Type type);
    friend std::ostream &operator<<(std::ostream &outs, const Token &tok);
    friend std::ostream &operator<<(std::ostream &outs, const Token *tok);

private:
    Type _type;
    std::string _text;
    static const std::unordered_map<Type, std::string> typeToStringMap;
};

#endif // TOKEN_HPP