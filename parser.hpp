#ifndef PARSER_HPP
#define PARSER_HPP

#include "scanner.hpp"
#include "exp.hpp"

class Parser
{
private:
    Scanner *scanner;
    Token *current, *previous;
    bool match(Token::Type ttype);
    bool check(Token::Type ttype);
    bool advance();
    bool isAtEnd();
    list<Stm *> parseStmList();
    Exp *parseCExp();
    Exp *parseExpression();
    Exp *parseTerm();
    Exp *parseFactor();

    void throwUnrecognizedTokenError(string message);

public:
    Stm *parseReturnStatement();
    Stm *parseForStatement();
    Stm *parseIfStatement();
    Stm *parsePrintStatement();
    Stm *parseAssignStatement();
    Stm *parseFCallStatement();
    Stm *parseWhileStatement();
    Stm *parseDoWhileStatement();

    Parser(Scanner *scanner);
    Program *parseProgram();
    Stm *parseStatement();
    StatementList *parseStatementList();
    VarDec *parseVarDec();
    VarDecList *parseVarDecList();
    Body *parseBody();
    FunDecList *parseFunDecList();
    FunDec *parseFunDec();
};

#endif // PARSER_HPP