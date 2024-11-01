#include <unordered_map>
#include <string>
#include <iostream>

class Token
{
public:
    enum Type
    {
        ERROR,             //
        ADD,               // +
        SUB,               // -
        MUL,               // *
        DIV,               // /
        LEFT_PARENTHESIS,  // (
        RIGHT_PARENTHESIS, // )
        FUN,               // fun
        LEFT_BRACKETS,     // {
        RIGHT_BRACKETS,    // }
        ASSIGN,            // =
        COLON,             // :
        INT,               // Int
        VAR,               // var
        COMMENT,           // //
        COMMENT_START,     // /*
        COMMENT_END,       // */
        PRINTLN,           // println
        COMMA,             // ,
        RETURN,            // return
        VAL,               // val
        POINT,             // .
        IF,                // if
        ELSE,              // else
        GREATER_THAN,      // >
        LESS_THAN,         // <
        GREATER_EQUAL,     // >=
        LESS_EQUAL,        // <=
        EQUAL,             // ==
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