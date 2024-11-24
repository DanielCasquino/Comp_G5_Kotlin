#include <iostream>
#include <stdexcept>
#include "token.hpp"
#include "scanner.hpp"
#include "exp.hpp"
#include "parser.hpp"

using namespace std;

void Parser::throw_invalidParse(Token::Type expected, Token::Type found, string message = "")
{
    cout << "Parser error: expected type (" << Token::typeToString(expected) << "), found (" << Token::typeToString(found) << ") at " << scanner->printPosition() << endl;
    if (message != "")
        cout << message << endl;
    exit(2);
}

bool Parser::match(Token::Type ttype)
{
    if (check(ttype))
    {
        advance();
        return true;
    }
    return false;
}

bool Parser::check(Token::Type ttype)
{
    if (isAtEnd())
        return false;
    return current->getType() == ttype;
}

bool Parser::advance()
{
    if (isAtEnd())
        return false;

    Token *temp = current;
    if (previous)
        delete previous;
    current = scanner->nextToken();
    previous = temp;
    if (check(Token::ERROR))
        scanner->throw_unrecognizedCharacter();
    return true;
}

bool Parser::isAtEnd()
{
    return current->getType() == Token::END;
}

Parser::Parser(Scanner *sc) : scanner(sc)
{
    previous = nullptr;
    current = scanner->nextToken();
    if (current->getType() == Token::ERROR)
        scanner->throw_unrecognizedCharacter();
}

Exp *Parser::parseCExp()
{
    Exp *left = parseExpression();
    if (match(Token::GREATER_THAN) || match(Token::GREATER_EQUAL) || match(Token::EQUAL))
    {
        BinaryOp op;
        if (previous->getType() == Token::GREATER_THAN)
        {
            op = LT_OP;
        }
        else if (previous->getType() == Token::GREATER_EQUAL)
        {
            op = LE_OP;
        }
        else if (previous->getType() == Token::EQUAL)
        {
            op = EQ_OP;
        }
        Exp *right = parseExpression();
        left = new BinaryExp(left, right, op);
    }
    return left;
}

Exp *Parser::parseExpression()
{
    Exp *left = parseTerm();
    while (match(Token::ADD) || match(Token::SUB))
    {
        BinaryOp op;
        if (previous->getType() == Token::ADD)
        {
            op = PLUS_OP;
        }
        else if (previous->getType() == Token::SUB)
        {
            op = MINUS_OP;
        }
        Exp *right = parseTerm();
        left = new BinaryExp(left, right, op);
    }
    return left;
}

Exp *Parser::parseTerm()
{
    Exp *left = parseFactor();
    while (match(Token::MUL) || match(Token::DIV))
    {
        BinaryOp op;
        if (previous->getType() == Token::MUL)
        {
            op = MUL_OP;
        }
        else if (previous->getType() == Token::DIV)
        {
            op = DIV_OP;
        }
        Exp *right = parseFactor();
        left = new BinaryExp(left, right, op);
    }
    return left;
}

Exp *Parser::parseFactor()
{
    Exp *e;
    Exp *e1;
    Exp *e2;
    if (match(Token::NUM))
    {
        return new NumberExp(stoi(previous->getText()));
    }
    else if (match(Token::ID))
    {
        string id = previous->getText();
        if (match(Token::LEFT_PARENTHESIS))
        { // Para arglis
            FCallExp *f_call_exp = new FCallExp(id);
            // if (check(Token::RIGHT_PARENTHESIS)) {//Ningun parametro
            //     // match(Token::RIGHT_PARENTHESIS);
            //     return f_call_exp;
            // }
            // Uno o mas parametros
            f_call_exp->arglist.push_back(parseExpression());
            while (match(Token::COMMA))
            {
                f_call_exp->arglist.push_back(parseExpression());
            }
            if (!match(Token::RIGHT_PARENTHESIS))
            {
                cerr << "Se esperaba un RIGHT_PARENTHESISf, pero se encontro_ " << current->getText() << endl;
                exit(9);
            }
            return f_call_exp;
        }
        return new IdentifierExp(previous->getText());
    }
    else if (match(Token::LEFT_PARENTHESIS))
    {
        e = parseCExp();
        if (!match(Token::RIGHT_PARENTHESIS))
        {
            cout << "Falta paréntesis derecho" << endl;
            exit(0);
        }
        return e;
    }
    cout << "Error: se esperaba un número o identificador pero se encontro___" << current->getText() << "Y antes estaba un__" << previous->getText() << endl;
    exit(0);
}

#pragma region Statements
Stm *Parser::parseAssignStatement()
{
    string id = previous->getText();
    if (!match(Token::ASSIGN))
        throw_invalidParse(Token::ASSIGN, current->getType(), "ParseAssignStatement: Expected '=' after identifier.");
    Exp *e = parseCExp();
    return new AssignStatement(id, e);
}

Stm *Parser::parsePrintStatement()
{
    if (!match(Token::LEFT_PARENTHESIS))
        throw_invalidParse(Token::LEFT_PARENTHESIS, current->getType(), "ParsePrintStatement: Expected '(' after 'println'.");
    Exp *e = parseCExp();
    if (!match(Token::RIGHT_PARENTHESIS))
        throw_invalidParse(Token::RIGHT_PARENTHESIS, current->getType(), "ParsePrintStatement: Expected ')' after expression inside println.");
    return new PrintStatement(e);
}

Stm *Parser::parseIfStatement()
{
    Exp *e = parseCExp();

    if (!match(Token::LEFT_BRACKET))
        throw_invalidParse(Token::LEFT_BRACKET, current->getType(), "ParseIfStatement: Expected '{' before then body.");
    Body *thenBody = parseBody();
    if (!match(Token::RIGHT_BRACKET))
        throw_invalidParse(Token::RIGHT_BRACKET, current->getType(), "ParseIfStatement: Expected '}' after then body.");

    if (!match(Token::ELSE))
        return new IfStatement(e, thenBody, nullptr);

    if (!match(Token::LEFT_BRACKET))
        throw_invalidParse(Token::LEFT_BRACKET, current->getType(), "ParseIfStatement: Expected '{' before else body.");
    Body *elseBody = parseBody();
    if (!match(Token::RIGHT_BRACKET))
        throw_invalidParse(Token::RIGHT_BRACKET, current->getType(), "ParseIfStatement: Expected '}' after else body.");
    return new IfStatement(e, thenBody, elseBody);
}

Stm *Parser::parseForStatement()
{
    if (!match(Token::LEFT_PARENTHESIS))
        throw_invalidParse(Token::LEFT_PARENTHESIS, current->getType(), "ParseForStatement: Expected '(' before 'for' expression.");

    if (!match(Token::ID))
        throw_invalidParse(Token::ID, current->getType(), "ParseForStatement: Expected temp variable after '('.");
    string temporalVariableId = previous->getText();
    if (!match(Token::IN))
        throw_invalidParse(Token::IN, current->getType(), "ParseForStatement: Expected 'in' after temp variable.");

    Exp *start = parseCExp();
    Exp *end = nullptr;
    Exp *step = new NumberExp(1);
    bool isUpTo = true;

    if (match(Token::DOUBLE_DOT)) // 1..5
        end = parseCExp();
    else if (match(Token::DOWNTO) || match(Token::UPTO)) // 1 upTo 5 step 2
    {
        isUpTo = previous->getType() == Token::UPTO;
        end = parseCExp();
        if (!match(Token::STEP))
            throw_invalidParse(Token::STEP, current->getType(), "ParseForStatement: Expected 'step' after end expression.");
        step = parseCExp();
    }
    else
        throw_invalidParse(Token::DOUBLE_DOT, current->getType(), "ParseForStatement: Expected '..' or 'upTo' or 'downTo' after start expression.");

    if (!match(Token::RIGHT_PARENTHESIS))
        throw_invalidParse(Token::RIGHT_PARENTHESIS, current->getType(), "ParseForStatement: Expected ')' after 'for' expression.");

    if (!match(Token::LEFT_BRACKET))
        throw_invalidParse(Token::LEFT_BRACKET, current->getType(), "ParseForStatement: Expected '{' before body.");

    Body *body = parseBody();

    if (!match(Token::RIGHT_BRACKET))
        throw_invalidParse(Token::RIGHT_BRACKET, current->getType(), "ParseForStatement: Expected '}' after body.");

    return new ForStatement(start, end, step, isUpTo, body);
}

Stm *Parser::parseReturnStatement()
{
    if (!match(Token::LEFT_PARENTHESIS))
        throw_invalidParse(Token::LEFT_PARENTHESIS, current->getType(), "ParseReturnStatement: Expected '(' after 'return'.");

    if (match(Token::RIGHT_PARENTHESIS)) // No stuff inside return
        return new ReturnStatement(nullptr);

    Exp *e = parseCExp();

    if (!match(Token::RIGHT_PARENTHESIS))
        throw_invalidParse(Token::RIGHT_PARENTHESIS, current->getType(), "ParseReturnStatement: Expected ')' after expression inside return.");

    return new ReturnStatement(e);
}

Stm *Parser::parseStatement()
{
    Stm *s = nullptr;

    if (current == nullptr)
        throw_invalidParse(Token::ID, Token::END, "ParseStatement: Expected a Statement, found nothing.");

    if (match(Token::ID))
        s = parseAssignStatement();

    else if (match(Token::PRINTLN))
        s = parsePrintStatement();

    else if (match(Token::IF))
        s = parseIfStatement();

    else if (match(Token::FOR))
        s = parseForStatement();

    else if (match(Token::RETURN))
        s = parseReturnStatement();
    else
        throw_invalidParse(Token::ID, current->getType(), "ParseStatement: Expected a Statement.");

    return s;
}
#pragma endregion

StatementList *Parser::parseStatementList()
{
    StatementList *sl = new StatementList();
    sl->add(parseStatement());
    while (match(Token::SEMICOLON))
        sl->add(parseStatement());
    return sl;
}

Body *Parser::parseBody()
{
    VarDecList *vdl = parseVarDecList();
    StatementList *sl = parseStatementList();
    return new Body(vdl, sl);
}

std::pair<string, string> Parser::parseParamDec()
{
    string type;
    string id;
    if (!match(Token::ID))
        throw_invalidParse(Token::ID, current->getType(), "ParamDec: Expected type after '('.");

    type = previous->getText();

    if (!match(Token::ID))
        throw_invalidParse(Token::ID, current->getType(), "ParamDec: Expected id after type.");

    id = previous->getText();

    return make_pair(type, id);
}

ParamDecList *Parser::parseParamDecList()
{
    ParamDecList *pdl = new ParamDecList;
    string type;
    string id;

    if (check(Token::RIGHT_PARENTHESIS)) // Empty ParamDecList
        return pdl;

    std::pair<string, string> first = parseParamDec();
    pdl->types.push_back(first.first);
    pdl->ids.push_back(first.second);

    while (match(Token::COMMA))
    {
        std::pair<string, string> next = parseParamDec();
        pdl->types.push_back(next.first);
        pdl->ids.push_back(next.second);
    }
    return pdl;
}

FunDec *Parser::parseFunDec()
{
    if (!match(Token::FUN)) // If no FUN, no FunDec
        return nullptr;

    if (!match(Token::ID)) // Parse id
        throw_invalidParse(Token::ID, current->getType(), "FunDec: Expected function name after type.");

    string id = previous->getText(); // Save function name

    if (!match(Token::LEFT_PARENTHESIS)) // Parse left parenthesis
        throw_invalidParse(Token::LEFT_PARENTHESIS, current->getType(), "FunDec: Expected '(' after function name.");

    ParamDecList *pdl = parseParamDecList();

    if (!match(Token::RIGHT_PARENTHESIS)) // Parse right parenthesis
        throw_invalidParse(Token::RIGHT_PARENTHESIS, current->getType(), "FunDec: Expected ')' after parameters.");

    if (!match(Token::COLON)) // Parse colon
        throw_invalidParse(Token::COLON, current->getType(), "FunDec: Expected ':' after parameters and before return type.");

    if (!match(Token::ID)) // Parse type
        throw_invalidParse(Token::ID, current->getType(), "FunDec: Expected function return type after FUN.");
    string type = previous->getText(); // Save function type

    if (!match(Token::LEFT_BRACKET)) // Parse start of function
        throw_invalidParse(Token::LEFT_BRACKET, current->getType(), "FunDec: Expected '{' at the start.");

    Body *body = parseBody();

    if (!match(Token::RIGHT_BRACKET)) // Parse end of function
        throw_invalidParse(Token::RIGHT_BRACKET, current->getType(), "FunDec: Expected '}' at the end.");

    return new FunDec(type, id, pdl, body);
}

FunDecList *Parser::parseFunDecList()
{
    FunDecList *fdl = new FunDecList();
    FunDec *first = parseFunDec();
    while (first) // Same as below, if we got a FunDec
    {
        fdl->add(first);       // Add it to the list
        first = parseFunDec(); // Try to parse another FunDec
    }
    return fdl;
}

// TODO: handle assignation at declaration
// For example, it works if we do:
// var x: Int
// x = 5
// But it doesn't work if we do:
// var x = 1
// Basically doesn't do type inference
VarDec *Parser::parseVarDec()
{
    if (!match(Token::VAR)) // Var keyword
        return nullptr;     // No VAR parsed, so there's no VarDec

    if (!match(Token::ID)) // Variable type
        throw_invalidParse(Token::ID, current->getType(), "VarDec: Expected variable type after VAR.");

    string name = previous->getText(); // Save variable name

    if (!match(Token::COLON)) // Colon after variable name
        throw_invalidParse(Token::COLON, current->getType(), "VarDec: Expected colon after variable name.");

    if (!match(Token::ID)) // Variable type
        throw_invalidParse(Token::ID, current->getType(), "VarDec: Expected variable type after colon.");

    string type = previous->getText(); // Save variable type
    list<string> variables;
    variables.push_back(name);
    return new VarDec(type, variables);
}

VarDecList *Parser::parseVarDecList()
{
    VarDecList *vdl = new VarDecList();
    VarDec *first = parseVarDec();
    while (first) // If we got a VarDec
    {
        vdl->add(first);       // Add it to the list
        first = parseVarDec(); // Try to parse another VarDec
    }
    return vdl;
}

Program *Parser::parseProgram()
{
    VarDecList *vdl = parseVarDecList();
    FunDecList *fdl = parseFunDecList();
    return new Program(vdl, fdl);
}