#ifndef PARSER_HPP
#define PARSER_HPP

#include "scanner.hpp"
#include "exp.hpp"

class Parser
{
private:
    Scanner *scanner;
    Token *current, *previous;
    void throw_invalidParse(Token::Type expected, Token::Type found, std::string message);
    bool match(Token::Type ttype);
    bool check(Token::Type ttype);
    bool advance();
    bool isAtEnd();
    Exp *parseCExp();
    Exp *parseExpression();
    Exp *parseTerm();
    Exp *parseFactor();

    // ParseStm dependencies
    Stm *parseReturnStatement();
    Stm *parseForStatement();
    Stm *parseIfStatement();
    Stm *parsePrintStatement();
    Stm *parseAssignStatement();

    // Body dependencies
    Stm *parseStatement();
    StatementList *parseStatementList();

    // FunDec dependencies
    Body *parseBody();
    std::pair<string, string> parseParamDec();
    ParamDecList *parseParamDecList();

    // Program dependencies
    FunDec *parseFunDec();
    FunDecList *parseFunDecList();
    //
    VarDec *parseVarDec();
    VarDecList *parseVarDecList();

public:
    Parser(Scanner *scanner);
    Program *parseProgram();
};

#endif // PARSER_HPP