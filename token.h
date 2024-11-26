#ifndef TOKEN_H
#define TOKEN_H

#include <string>

class Token
{
public:
    enum Type
    {
        PLUS,
        MINUS,
        MUL,
        DIV,
        NUM,
        ERR,
        END,
        ASSIGN,
        IF,
        ELSE,
        COMA,
        FOR,
        RETURN,

        GREATER_THAN,  // >
        GREATER_EQUAL, // >=
        EQUAL,         // ==
        LESS_THAN,     // <
        LESS_EQUAL,    // <=

        FUN,    // fun
        ENDFUN, // endfun

        PD, // (
        PI, // )

        VAL, // val
        VAR, // var
        ID,  // id

        COLON,          //:
        PRINTLN,        // println
        LEFT_BRACKETS,  // {
        RIGHT_BRACKETS, // }

        LINE_BREAK, // \n
        PC,         // ;
        DDOT,       // ..
        IN,         // in
        WHILE
    };

    Type type;
    std::string text;

    Token(Type type);
    Token(Type type, char c);
    Token(Type type, std::string c);
    Token(Type type, const std::string &source, int first, int last);

    Token(const Token &other);
    Token &operator=(const Token &other);

    friend std::ostream &operator<<(std::ostream &outs, const Token &tok);
    friend std::ostream &operator<<(std::ostream &outs, const Token *tok);
};

#endif // TOKEN_H