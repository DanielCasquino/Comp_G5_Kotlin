#include <iostream>
#include <stdexcept>
#include "token.hpp"
#include "scanner.hpp"
#include "exp.hpp"
#include "parser.hpp"

using namespace std;

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
        {
            cout << "Error de análisis, carácter no reconocido: " << current->text << endl;
            exit(1);
        }
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
    previous = NULL;
    current = scanner->nextToken();
    if (current->getType() == Token::ERROR)
    {
        cout << "Error en el primer token: " << current->getText() << endl;
        exit(1);
    }
}

VarDec *Parser::parseVarDec()
{
    VarDec *vd = NULL;
    if (match(Token::VAR))
    {
        if (!match(Token::ID))
        {
            cout << "Error: se esperaba un identificador después de 'var'." << endl;
            exit(1);
        }
        string type = previous->getText();
        list<string> ids;
        if (!match(Token::ID))
        {
            cout << "Error: se esperaba un identificador después de 'var'." << endl;
            exit(1);
        }
        ids.push_back(previous->getText());
        while (match(Token::COMMA))
        {
            if (!match(Token::ID))
            {
                cout << "Error: se esperaba un identificador después de ','." << endl;
                exit(1);
            }
            ids.push_back(previous->getText());
        }
        if (!match(Token::SEMICOLON))
        {
            cout << "Error: se esperaba un ';' al final de la declaración." << endl;
            exit(1);
        }
        vd = new VarDec(type, ids);
    }
    return vd;
}

VarDecList *Parser::parseVarDecList()
{
    VarDecList *vdl = new VarDecList();
    VarDec *aux;
    aux = parseVarDec();
    while (aux != NULL)
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
    while (match(Token::SEMICOLON))
    {
        sl->add(parseStatement());
    }
    return sl;
}

Body *Parser::parseBody()
{
    VarDecList *vdl = parseVarDecList();
    StatementList *sl = parseStatementList();
    return new Body(vdl, sl);
}

ParamDecList *Parser::parseParamDecList()
{
    ParamDecList *param_dec_list = new ParamDecList;
    string type;
    string id;
    if (check(Token::RIGHT_PARENTHESIS))
    { // Parametros sin argumentos para el main
        return param_dec_list;
    }

    if (!match(Token::ID))
    {
        cerr << "Fatal el ID_TYPE" << endl;
        exit(6);
    }
    type = previous->getText();
    if (!match(Token::ID))
    {
        cerr << "Fatal el ID_ID" << endl;
        exit(2);
    }
    id = previous->getText();
    param_dec_list->types.push_back(type);
    param_dec_list->ids.push_back(id);

    while (match(Token::COMMA))
    {
        if (!match(Token::ID))
        {
            cerr << "Fatal el ID_TYPE" << endl;
            exit(6);
        }
        type = previous->getText();
        if (!match(Token::ID))
        {
            cerr << "Fatal el ID_ID" << endl;
            exit(2);
        }
        id = previous->getText();
        param_dec_list->types.push_back(type);
        param_dec_list->ids.push_back(id);
    }
    return param_dec_list;
}

FunDec *Parser::parseFunDec()
{
    FunDec *fd = NULL;
    if (match(Token::FUN))
    {
        string id;
        string type;
        if (!match(Token::ID))
        {
            cerr << "Falta el token ID" << endl;
            exit(2);
        }
        type = previous->getText();
        if (!match(Token::ID))
        {
            cerr << "Falta el token id" << endl;
            exit(3);
        }
        id = previous->getText();
        if (!match(Token::LEFT_PARENTHESIS))
        {
            cerr << "Falta el token LEFT_PARENTHESIS" << endl;
            exit(4);
        }
        ParamDecList *param_dec_list = parseParamDecList();
        if (!match(Token::RIGHT_PARENTHESIS))
        {
            cerr << "Falta el token RIGHT_PARENTHESIS" << endl;
            cout << "Curren token es: " << current->getText() << endl;
            exit(5);
        }
        Body *body = parseBody();
        // cout<<"El current token es::: "<<current->text<<endl;
        if (!match(Token::ENDFUN))
        {
            cerr << "Falta el token ENDFUN, se encontro:" << current->getText() << endl;
            exit(1);
        }
        fd = new FunDec(type, id, param_dec_list, body);
    }
    return fd;
}

FunDecList *Parser::parseFunDecList()
{
    FunDecList *fdl = new FunDecList();
    FunDec *aux;
    aux = parseFunDec();
    while (aux != NULL)
    {
        fdl->add(aux);
        aux = parseFunDec();
    }
    return fdl;
}

Program *Parser::parseProgram()
{
    VarDecList *var_dec_list = parseVarDecList();
    FunDecList *fun_dec_list = parseFunDecList();
    return new Program(var_dec_list, fun_dec_list);
}

list<Stm *> Parser::parseStmList()
{
    list<Stm *> slist;
    slist.push_back(parseStatement());
    while (match(Token::SEMICOLON))
    {
        slist.push_back(parseStatement());
    }
    return slist;
}

Stm *Parser::parseStatement()
{
    Stm *s = NULL;
    Exp *e = NULL;
    Body *tb = NULL; // true case
    Body *fb = NULL; // false case
    // cout<<"El current token en parStatement es:_"<<current->text<<endl;
    if (current == NULL)
    {
        cout << "Error: Token actual es NULL" << endl;
        exit(1);
    }

    if (match(Token::ID))
    {
        string lex = previous->text;

        if (!match(Token::ASSIGN))
        {
            cout << "Error: se esperaba un '=' después del identificador." << endl;
            exit(1);
        }
        e = parseCExp();
        s = new AssignStatement(lex, e);
    }
    else if (match(Token::PRINTLN))
    {
        if (!match(Token::PI))
        {
            cout << "Error: se esperaba un '(' después de 'print'." << endl;
            exit(1);
        }
        e = parseCExp();
        if (!match(Token::RIGHT_PARENTHESIS))
        {
            cout << "Error: se esperaba un ')' después de la expresión." << endl;
            exit(1);
        }
        s = new PrintStatement(e);
    }
    else if (match(Token::IF))
    {
        e = parseCExp();
        if (!match(Token::LEFT_BRACKETS))
        {
            cout << "Error: se esperaba 'then' después de la expresión." << endl;
            exit(1);
        }

        tb = parseBody();

        if (match(Token::ELSE))
        {
            fb = parseBody();
        }
        if (!match(Token::RIGHT_BRACKETS))
        {
            cout << "Error: se esperaba 'end' al final de la declaración." << endl;
            exit(1);
        }
        s = new IfStatement(e, tb, fb);
    }
    else if (match(Token::FOR))
    {
        if (!match(Token::PI))
        {
            cout << "Error: se esperaba '(' después de 'for'." << endl;
            exit(1);
        }
        Exp *start = parseCExp();
        if (!match(Token::COMA))
        {
            cout << "Error: se esperaba ',' después de la expresión." << endl;
            exit(1);
        }
        Exp *end = parseCExp();
        if (!match(Token::COMA))
        {
            cout << "Error: se esperaba ',' después de la expresión." << endl;
            exit(1);
        }
        Exp *step = parseCExp();
        if (!match(Token::RIGHT_PARENTHESIS))
        {
            cout << "Error: se esperaba ')' después de la expresión." << endl;
            exit(1);
        }
        tb = parseBody();
        if (!match(Token::RIGHT_BRACKETS))
        {
            cout << "Error: se esperaba 'endfor' al final de la declaración." << endl;
            exit(1);
        }
        s = new ForStatement(start, end, step, tb);
    }
    else if (match(Token::RETURN))
    {
        if (!match(Token::PI))
        {
            cout << "Error: se esperaba '(' después de 'for'." << endl;
            exit(1);
        }
        if (check(Token::RIGHT_PARENTHESIS))
        { // Caso del return vacio
            match(Token::RIGHT_PARENTHESIS);
            return new ReturnStatement();
        }
        Exp *CExp = parseCExp();

        if (!match(Token::RIGHT_PARENTHESIS))
        {
            cout << "Error: se esperaba ')' después de la expresión." << endl;
            exit(1);
        }
        s = new ReturnStatement(CExp);
    }
    else
    {
        cout << "Error: Se esperaba un identificador o 'print', pero se encontró: " << *current << endl;
        exit(1);
    }

    return s;
}

Exp *Parser::parseCExp()
{
    Exp *left = parseExpression();
    if (match(Token::GREATER_THAN) || match(Token::GREATER_EQUAL) || match(Token::EQUAL))
    {
        BinaryOp op;
        if (previous->type == Token::GREATER_THAN)
        {
            op = LT_OP;
        }
        else if (previous->type == Token::GREATER_EQUAL)
        {
            op = LE_OP;
        }
        else if (previous->type == Token::EQUAL)
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
    while (match(Token::PLUS) || match(Token::MINUS))
    {
        BinaryOp op;
        if (previous->type == Token::PLUS)
        {
            op = PLUS_OP;
        }
        else if (previous->type == Token::MINUS)
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
        if (previous->type == Token::MUL)
        {
            op = MUL_OP;
        }
        else if (previous->type == Token::DIV)
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
        return new NumberExp(stoi(previous->text));
    }
    else if (match(Token::ID))
    {
        string id = previous->text;
        if (match(Token::PI))
        { // Para arglis
            FCallExp *f_call_exp = new FCallExp(id);
            // if (check(Token::RIGHT_PARENTHESIS)) {//Ningun parametro
            //     // match(Token::RIGHT_PARENTHESIS);
            //     return f_call_exp;
            // }
            // Uno o mas parametros
            f_call_exp->arglist.push_back(parseExpression());
            while (match(Token::COMA))
            {
                f_call_exp->arglist.push_back(parseExpression());
            }
            if (!match(Token::RIGHT_PARENTHESIS))
            {
                cerr << "Se esperaba un RIGHT_PARENTHESISf, pero se encontro_ " << current->text << endl;
                exit(9);
            }
            return f_call_exp;
        }
        return new IdentifierExp(previous->text);
    }
    else if (match(Token::PI))
    {
        e = parseCExp();
        if (!match(Token::RIGHT_PARENTHESIS))
        {
            cout << "Falta paréntesis derecho" << endl;
            exit(0);
        }
        return e;
    }
    cout << "Error: se esperaba un número o identificador pero se encontro___" << current->text << "Y antes estaba un__" << previous->text << endl;
    exit(0);
}
