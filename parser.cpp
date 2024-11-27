#include <iostream>
#include <stdexcept>
#include "token.hpp"
#include "scanner.hpp"
#include "exp.hpp"
#include "parser.hpp"

using namespace std;

void Parser::throwUnrecognizedTokenError(string message = "")
{
    cout << "Error de análisis, carácter no reconocido: " << current->getText() << endl;
    if (message != "")
        cout << message << endl;
    exit(1);
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
    if (!isAtEnd())
    {
        Token *temp = current;
        if (previous)
            delete previous;
        current = scanner->nextToken();
        previous = temp;
        if (check(Token::ERROR))
            throwUnrecognizedTokenError();
        return true;
    }
    return false;
}

bool Parser::isAtEnd()
{
    return (current->getType() == Token::END);
}

Parser::Parser(Scanner *sc) : scanner(sc)
{
    previous = nullptr;
    current = scanner->nextToken();
    if (current->getType() == Token::ERROR)
        throwUnrecognizedTokenError();
}

VarDec *Parser::parseVarDec()
{
    if (!(match(Token::VAR) || match(Token::VAL)))
        return nullptr;

    string valVar = previous->getText();

    list<string> ids;

    if (!match(Token::ID))
        throwUnrecognizedTokenError("ParseVarDec: Expected ID after var/val");
    ids.push_back(previous->getText());

    while (match(Token::COMMA))
    {
        if (!match(Token::ID))
            throwUnrecognizedTokenError("ParseVarDec: Expected ID after COMMA");
        ids.push_back(previous->getText());
    }

    if (!match(Token::COLON))
        throwUnrecognizedTokenError("ParseVarDec: Expected COLON after last ID");

    if (!match(Token::ID))
        throwUnrecognizedTokenError("ParseVarDec: Expected ID (for the type) after COLON");

    string type = previous->getText(); // el type es un id

    if (!match(Token::SEMICOLON) && !match(Token::LINE_BREAK))
        throwUnrecognizedTokenError("ParseVarDec: Expected LINEBREAK after end of declaration");

    return new VarDec(type, ids, valVar);
}

VarDecList *Parser::parseVarDecList()
{
    VarDecList *vdl = new VarDecList();
    VarDec *aux;
    aux = parseVarDec();
    while (aux)
    {
        vdl->add(aux);
        aux = parseVarDec();
    }
    return vdl;
}

StatementList *Parser::parseStatementList()
{
    StatementList *sl = new StatementList();
    sl->add(parseStatement());
    while (match(Token::SEMICOLON) || match(Token::LINE_BREAK))
    {
        // Si encuentra un RIGHT_BRACKET break y el salto de linea muere
        if (check(Token::RIGHT_BRACKET))
            break;
        sl->add(parseStatement());
    }
    return sl;
}

Body *Parser::parseBody()
{
    VarDecList *vdl = parseVarDecList();
    StatementList *sl = parseStatementList();
    cout << "Correcto: " << previous->getText() << endl;
    return new Body(vdl, sl);
}

FunDec *Parser::parseFunDec()
{
    FunDec *fd = nullptr;

    if (!match(Token::FUN))
        return nullptr;

    match(Token::LINE_BREAK);

    Body *body = nullptr;

    if (!match(Token::ID))
        throwUnrecognizedTokenError("ParseFunDec: Expected ID after FUN");

    string fname = previous->getText();
    string rtype = "void"; // Tipo void por defecto

    match(Token::LINE_BREAK);

    list<string> types;
    list<string> vars;

    if (!match(Token::LEFT_PARENTHESIS))
        throwUnrecognizedTokenError("ParseFunDec: Expected LEFT_PARENTHESIS after ID");

    if (!check(Token::RIGHT_PARENTHESIS))
    { // Entro a la lista de argumentos
        if (!match(Token::ID))
            throwUnrecognizedTokenError("ParseFunDec: Expected ID after LEFT_PARENTHESIS");

        vars.push_back(previous->getText());

        if (!match(Token::COLON))
            throwUnrecognizedTokenError("ParseFunDec: Expected COLON after ID");
        if (!match(Token::ID))
            throwUnrecognizedTokenError("ParseFunDec: Expected ID after COLON");

        types.push_back(previous->getText());

        while (match(Token::COMMA))
        {
            if (!match(Token::ID))
                throwUnrecognizedTokenError("ParseFunDec: Expected ID after COMMA");
            vars.push_back(previous->getText());
            if (!match(Token::COLON))
                throwUnrecognizedTokenError("ParseFunDec: Expected COLON after ID");

            if (!match(Token::ID))
                throwUnrecognizedTokenError("ParseFunDec: Expected ID after COLON");
            types.push_back(previous->getText());
        }
    }
    if (!match(Token::RIGHT_PARENTHESIS))
        throwUnrecognizedTokenError("ParseFunDec: Expected RIGHT_PARENTHESIS after arguments");
    // Cheakeamos si hay o no la declaracion del tipo de la funcion:
    if (check(Token::COLON))
    {
        match(Token::COLON); // Matamos colon
        if (match(Token::ID))
        {                                // Funcion con tipo
            rtype = previous->getText(); // Le damos el tipo d ela funcion
        }
    }

    // Verificamos la presencia de saltos de línea:
    // matcheamos pq puede haber un salto de linea despues de {
    match(Token::LINE_BREAK);

    if (!match(Token::LEFT_BRACKET))
        throwUnrecognizedTokenError("ParseFunDec: Expected LEFT_BRACKET before body");
    body = parseBody();

    if (!match(Token::RIGHT_BRACKET))
        throwUnrecognizedTokenError("ParseFunDec: Expected RIGHT_BRACKET after body");

    return new FunDec(fname, types, vars, rtype, body);
}

FunDecList *Parser::parseFunDecList()
{
    FunDecList *fdl = new FunDecList();
    FunDec *aux;
    // Tner cuidado con los saltos de linea
    if (check(Token::LINE_BREAK))
    {
        // Lo matamos
        match(Token::LINE_BREAK);
    }

    aux = parseFunDec();
    while (aux != nullptr)
    {
        fdl->add(aux);
        if (check(Token::LINE_BREAK))
        {
            // Lo matamos
            match(Token::LINE_BREAK);
        }
        aux = parseFunDec();
    }
    return fdl;
}

Program *Parser::parseProgram()
{
    VarDecList *v = parseVarDecList();
    FunDecList *b = parseFunDecList();
    return new Program(v, b);
}

list<Stm *> Parser::parseStmList()
{
    list<Stm *> slist;
    slist.push_back(parseStatement());
    while (match(Token::SEMICOLON) || match(Token::LINE_BREAK)) // ojito
    {
        slist.push_back(parseStatement());
    }
    return slist;
}

Stm *Parser::parsePrintStatement()
{
    if (!match(Token::LEFT_PARENTHESIS))
        throwUnrecognizedTokenError("ParsePrintStatement: Expected LEFT_PARENTHESIS after println");
    Exp *e = parseCExp();
    if (!match(Token::RIGHT_PARENTHESIS))
        throwUnrecognizedTokenError("ParsePrintStatement: Expected RIGHT_PARENTHESIS after expression");
    return new PrintStatement(e);
}

Stm *Parser::parseIfStatement()
{
    match(Token::LINE_BREAK);

    Exp *e = parseCExp();

    // Puede haber un salto de línea
    match(Token::LINE_BREAK);

    if (!match(Token::LEFT_BRACKET))
        throwUnrecognizedTokenError("ParseIfStatement: Expected LEFT_BRACKET after if expression");

    Body *thenBody = parseBody();

    if (!match(Token::RIGHT_BRACKET))
        throwUnrecognizedTokenError("ParseIfStatement: Expected RIGHT_BRACKET after if body");

    match(Token::LINE_BREAK);

    if (!check(Token::ELSE))
        return new IfStatement(e, thenBody, nullptr);

    Body *elseBody = nullptr;

    if (!match(Token::ELSE))
        throwUnrecognizedTokenError("ParseIfStatement: Expected ELSE after if body");

    match(Token::LINE_BREAK);

    if (!match(Token::LEFT_BRACKET))
        throwUnrecognizedTokenError("ParseIfStatement: Expected LEFT_BRACKET after else");

    match(Token::LINE_BREAK);

    elseBody = parseBody();

    if (!match(Token::RIGHT_BRACKET))
        throwUnrecognizedTokenError("ParseIfStatement: Expected RIGHT_BRACKET after else body");

    return new IfStatement(e, thenBody, elseBody);
}

Stm *Parser::parseForStatement()
{
    Exp *step = new NumberExp(1); // Default step is 1
    bool isUpto = true;           // by default, ++i
    bool exStep = false;          // by default, not step keyword needed

    if (!match(Token::LEFT_PARENTHESIS))
        throwUnrecognizedTokenError("ParseForStatement: Expected LEFT_PARENTHESIS after for");

    if (!match(Token::ID))
        throwUnrecognizedTokenError("ParseForStatement: Expected variable ID after LEFT_PARENTHESIS");

    string tempVarId = previous->getText();

    if (!match(Token::IN))
        throwUnrecognizedTokenError("ParseForStatement: Expected IN after variable ID");

    Exp *start = parseCExp();
    if (!match(Token::DOUBLE_DOT))
    {
        if (!match(Token::DOWNTO))
            throwUnrecognizedTokenError("ParseForStatement: Expected DOUBLE_DOT or DOWNTO after start expression");
        isUpto = false;
    }

    Exp *end = parseCExp();

    if (match(Token::STEP))
    {
        exStep = true;
        step = parseCExp();
    }

    if (!match(Token::RIGHT_PARENTHESIS))
        throwUnrecognizedTokenError("ParseForStatement: Expected RIGHT_PARENTHESIS after end expression");

    match(Token::LINE_BREAK);

    if (!match(Token::LEFT_BRACKET))
        throwUnrecognizedTokenError("ParseForStatement: Expected LEFT_BRACKET before for body");

    // Al terner la variable temporal la guardamos con id i = 1, es un assing
    Exp *temporal = new IdentifierExp(tempVarId);
    // VarDec* temporal_variable_ = new VarDec("Int", {temporal_variable}, "var");
    Body *body = parseBody();
    // save the iterator in the body
    // auto iterator_assign = new AssignStatement(temporal_variable, start);
    // tb->slist->stms.push_front(iterator_assign);
    if (!match(Token::RIGHT_BRACKET))
        throwUnrecognizedTokenError("ParseForStatement: Expected RIGHT_BRACKET after for body");

    Stm *temporalVarAssign = new AssignStatement(tempVarId, start);

    return new ForStatement(start, end, step, body, tempVarId, temporalVarAssign, isUpto, exStep);
}

Stm *Parser::parseWhileStatement()
{
    Exp *e = parseCExp();
    match(Token::LINE_BREAK);

    if (!match(Token::LEFT_BRACKET))
        throwUnrecognizedTokenError("ParseWhileStatement: Expected LEFT_BRACKET after while expression");
    Body *body = parseBody();
    if (!match(Token::RIGHT_BRACKET))
        throwUnrecognizedTokenError("ParseWhileStatement: Expected RIGHT_BRACKET after while body");
    return new WhileStatement(e, body);
}

Stm *Parser::parseDoWhileStatement()
{
    match(Token::LINE_BREAK);

    if (!match(Token::LEFT_BRACKET))
        throwUnrecognizedTokenError("ParseDoWhileStatement: Expected LEFT_BRACKET after doWhile expression");
    Body *body = parseBody();
    if (!match(Token::RIGHT_BRACKET))
        throwUnrecognizedTokenError("ParseDoWhileStatement: Expected RIGHT_BRACKET after doWhile body");
    if (!match(Token::WHILE))
        throwUnrecognizedTokenError("ParseDoWhileStatement: Expected WHILE after doWhile body");
    if (!match(Token::LEFT_PARENTHESIS))
        throwUnrecognizedTokenError("ParseDoWhileStatement: Expected LEFT_PARENTHESIS after WHILE");
    Exp *e = parseCExp();
    if (!match(Token::RIGHT_PARENTHESIS))
        throwUnrecognizedTokenError("ParseDoWhileStatement: Expected RIGHT_PARENTHESIS after WHILE expression");
    return new DoWhileStatement(e, body);
}

Stm *Parser::parseReturnStatement()
{
    Exp *e = parseCExp();
    return new ReturnStatement(e); // Si es null, no hay problema
}

Stm *Parser::parseStatement()
{
    Stm *s = nullptr;
    Exp *e = nullptr;
    Body *tb = nullptr; // true case
    Body *fb = nullptr; // false case

    if (current == nullptr)
    {
        cout << "Error: Token actual es nullptr" << endl;
        exit(1);
    }

    if (match(Token::ID))
    {
        string lex = previous->getText();

        if (match(Token::ASSIGN))
        {
            e = parseCExp();
            s = new AssignStatement(lex, e);
        }
        else
        {
            if (match(Token::LEFT_PARENTHESIS))
            {
                list<Exp *> args;
                if (!check(Token::RIGHT_PARENTHESIS))
                {
                    args.push_back(parseCExp());
                    while (match(Token::COMMA))
                    {
                        args.push_back(parseCExp());
                    }
                }
                if (!match(Token::RIGHT_PARENTHESIS))
                {
                    cout << "Falta paréntesis derecho" << endl;
                    exit(0);
                }
                s = new FCallStatement(lex, args);
            }
        }
    }
    else if (match(Token::PRINTLN))
        s = parsePrintStatement();
    else if (match(Token::IF))
        s = parseIfStatement();
    else if (match(Token::FOR))
        s = parseForStatement();
    else if (match(Token::WHILE))
        s = parseWhileStatement();
    else if (match(Token::DO))
        s = parseDoWhileStatement();
    else if (match(Token::RETURN))
        s = parseReturnStatement();
    else
        throwUnrecognizedTokenError("ParseStatement: Expected statement");
    return s;
}

Exp *Parser::parseCExp()
{
    Exp *left = parseExpression();
    if (match(Token::GREATER_THAN) || match(Token::GREATER_EQUAL) || match(Token::LESS_THAN) || match(Token::LESS_EQUAL) || match(Token::EQUAL))
    {
        BinaryOp op;
        switch (previous->getType())
        {
        case Token::GREATER_THAN:
            op = GT_OP;
            break;
        case Token::GREATER_EQUAL:
            op = GE_OP;
            break;
        case Token::LESS_THAN:
            op = LT_OP;
            break;
        case Token::LESS_EQUAL:
            op = LE_OP;
            break;
        case Token::EQUAL:
            op = EQ_OP;
            break;
            // TODO: ADD NOT_EQUAL
        }
        Exp *right = parseExpression();
        match(Token::LINE_BREAK); // ojito1
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
        switch (previous->getType())
        {
        case Token::ADD:
            op = PLUS_OP;
            break;
        case Token::SUB:
            op = MINUS_OP;
            break;
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
        switch (previous->getType())
        {
        case Token::MUL:
            op = MUL_OP;
            break;
        case Token::DIV:
            op = DIV_OP;
            break;
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
        return new NumberExp(stoi(previous->getText()));
    else if (match(Token::ID))
    {
        string texto = previous->getText();
        // Parse FCallExp
        if (match(Token::LEFT_PARENTHESIS))
        {
            list<Exp *> args;
            if (!check(Token::RIGHT_PARENTHESIS))
            {
                args.push_back(parseCExp());
                while (match(Token::COMMA))
                {
                    args.push_back(parseCExp());
                }
            }
            if (!match(Token::RIGHT_PARENTHESIS))
            {
                cout << "Falta paréntesis derecho" << endl;
                exit(0);
            }
            return new FCallExp(texto, args);
        }
        else
        {
            return new IdentifierExp(previous->getText());
        }
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
    cout << "Error: se esperaba un número o identificador." << endl;
    exit(0);
}
