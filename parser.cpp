#include <iostream>
#include <stdexcept>
#include "token.h"
#include "scanner.h"
#include "exp.h"
#include "parser.h"

using namespace std;

bool Parser::match(Token::Type ttype) {
    if (check(ttype)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::check(Token::Type ttype) {
    if (isAtEnd()) return false;
    return current->type == ttype;
}

bool Parser::advance() {
    if (!isAtEnd()) {
        Token* temp = current;
        if (previous) delete previous;
        current = scanner->nextToken();
        previous = temp;
        if (check(Token::ERR)) {
            cout << "Error de análisis, carácter no reconocido: " << current->text << endl;
            exit(1);
        }
        return true;
    }
    return false;
}

bool Parser::isAtEnd() {
    return (current->type == Token::END);
}

Parser::Parser(Scanner* sc):scanner(sc) {
    previous = NULL;
    current = scanner->nextToken();
    if (current->type == Token::ERR) {
        cout << "Error en el primer token: " << current->text << endl;
        exit(1);
    }
}

VarDec* Parser::parseVarDec() {
    VarDec* vd = NULL;
    if (match(Token::VAR) || match(Token::VAL)) {
        string val_var = previous->text;

        //Ids
        list<string> ids;
        if (!match(Token::ID)) {
            cout << "Error: se esperaba un identificador después de 'var'." << endl;
            exit(1);
        }
        ids.push_back(previous->text);
        while (match(Token::COMA)) {
            if (!match(Token::ID)) {
                cout << "Error: se esperaba un identificador después de ','." << endl;
                exit(1);
            }
            ids.push_back(previous->text);
        } //ids es de tamaño 1 skjdks
        if (!match(Token::COLON)) {
            cout << "Error: se esperaba un ':'" << endl;
            exit(1);
        }
        if (!match(Token::ID)) {
            cout << "Error: se esperaba un identificador después de 'var' o 'val'." << endl;
            exit(1);
        }
        string type = previous->text; //Type es id

        if (!match(Token::PC) and !match(Token::LINE_BREAK) ) {
            cout << "Error: se esperaba un ';' un salto de linea al final de la declaración." << endl;
            exit(1);
        }
        vd = new VarDec(type, ids, val_var);
    }
    return vd;
}

VarDecList* Parser::parseVarDecList() {
    VarDecList* vdl = new VarDecList();
    VarDec* aux;
    aux = parseVarDec();
    while (aux != NULL) {
        vdl->add(aux);
        aux = parseVarDec();
    }
    return vdl;
}

StatementList* Parser::parseStatementList() {
    StatementList* sl = new StatementList();
    sl->add(parseStatement());
    while (match(Token::PC) or match(Token::LINE_BREAK) ) {
        //Si encuentra un RIGHT_BRACKETS break y el salto de linea muere
        if (check(Token::RIGHT_BRACKETS) ) {
            break;
        }
        sl->add(parseStatement());
    }
    return sl;
}


Body* Parser::parseBody() {
    VarDecList* vdl = parseVarDecList();
    StatementList* sl = parseStatementList();
    return new Body(vdl, sl);
}

FunDec* Parser::parseFunDec() {
  FunDec* fd = NULL;
  if (match(Token::FUN)) {
    Body* body = NULL;
    if (!match(Token::ID)) {
        cout << "Error: se esperaba el nombre de la funcion'." << endl;
        exit(1);
    }
    string fname = previous->text;
      string rtype = "void"; //Tipo void por defecto

      list<string> types;
      list<string> vars;
      if (!match(Token::PI)) {
          cout << "Error: se esperaba un '('" << endl;
          exit(1);
      }
      if (!check(Token::PD)) { //Entro a la lista de argumentos
          if (!match(Token::ID)) {
              cout << "Error: se esperaba un id después del '('." << endl;
              exit(1);
          }
          vars.push_back(previous->text);
          if (!match(Token::COLON)) {
              cout << "Error: se esperaba un ':'" << endl;
              exit(1);
          }
          if (!match(Token::ID)) {
              cout << "Error: se esperaba un tipo" << endl;
              exit(1);
          }
          types.push_back(previous->text);
          while(match(Token::COMA)) {
              if (!match(Token::ID)) {
                  cout << "Error: se esperaba un id después del '('." << endl;
                  exit(1);
              }
              vars.push_back(previous->text);
              if (!match(Token::COLON)) {
                  cout << "Error: se esperaba un ':'" << endl;
                  exit(1);
              }
              if (!match(Token::ID)) {
                  cout << "Error: se esperaba un tipo" << endl;
                  exit(1);
              }
              types.push_back(previous->text);
          }
      }
      if (!match(Token::PD)) {
          cout << "Error: se esperaba un ')' después de la lista de argumentos." << endl;
          exit(1);
      }
      //Cheakeamos si hay o no la declaracion del tipo de la funcion:
      if (check(Token::COLON)) {
          match(Token::COLON); //Matamos colon
          if(match(Token::ID)){ //Funcion con tipo
              rtype = previous->text; //Le damos el tipo d ela funcion
          }
      }

    if (!match(Token::LEFT_BRACKETS)) {
        cout << "Error: se esperaba un '{'" << endl;
        exit(1);
    }
    body = parseBody();

      if (!match(Token::RIGHT_BRACKETS)){
        cout << "Error: se esperaba '}' pero se tiente un: "<<current->text << endl;
        exit(1);
    }

    fd = new FunDec(fname, types, vars, rtype, body);
  }
  return fd;
}

FunDecList* Parser::parseFunDecList() {
    FunDecList* fdl = new FunDecList();
    FunDec* aux;
    //Tner cuidado con los saltos de linea
    if (check(Token::LINE_BREAK)) {
        //Lo matamos
        match(Token::LINE_BREAK);
    }

    aux = parseFunDec();
    while (aux != NULL) {
        fdl->add(aux);
        if (check(Token::LINE_BREAK)) {
            //Lo matamos
            match(Token::LINE_BREAK);
        }
        aux = parseFunDec();
    }
    return fdl;
}

Program* Parser::parseProgram() {
    VarDecList* v = parseVarDecList();
    FunDecList* b = parseFunDecList();
    return new Program(v, b);
}

list<Stm*> Parser::parseStmList() {
    list<Stm*> slist;
    slist.push_back(parseStatement());
    while(match(Token::PC)) {
        slist.push_back(parseStatement());
    }
    return slist;
}

Stm* Parser::parseStatement() {
    Stm* s = NULL;
    Exp* e = NULL;
    Body* tb = NULL; //true case
    Body* fb = NULL; //false case

    if (current == NULL) {
        cout << "Error: Token actual es NULL" << endl;
        exit(1);
    }

    if (match(Token::ID)) {
        string lex = previous->text;

        if (match(Token::ASSIGN)) {
            e = parseCExp();
            s = new AssignStatement(lex, e);
        }else{
            if(match(Token::PI)){
                list<Exp*> args;
                if (!check(Token::PD)){
                    args.push_back(parseCExp());
                    while (match(Token::COMA)){
                        args.push_back(parseCExp());
                    }
                }
                if (!match(Token::PD)){
                    cout << "Falta paréntesis derecho" << endl;
                    exit(0);
                }
                s = new FCallStatement(lex, args);
            }
        }

    } else if (match(Token::PRINTLN)) {
        if (!match(Token::PI)) {
            cout << "Error: se esperaba un '(' después de 'print'." << endl;
            exit(1);
        }
        e = parseCExp();
        if (!match(Token::PD)) {
            cout << "Error: se esperaba un ')' después de la expresión." << endl;
            exit(1);
        }
        s = new PrintStatement(e);
    }
    else if (match(Token::IF)) {
        e = parseCExp();
        if (!match(Token::LEFT_BRACKETS)) {
            cout << "Error: se esperaba '{' después de la expresión." << endl;
            exit(1);
        }
        // cout<<"Entrando a body"<<endl;
        tb = parseBody();
        // cout<<"Saliendo de body"<<endl;

        if (!match(Token::RIGHT_BRACKETS)) {
            cout << "Error: se esperaba '}' después de la expresión." << endl;
            exit(1);
        }

        if (match(Token::ELSE)) {
            if (!match(Token::LEFT_BRACKETS)) {
                cout << "Error: se esperaba '{' después de la expresión." << endl;
                exit(1);
            }
            fb = parseBody();
        }

        if (!match(Token::RIGHT_BRACKETS)) {
            cout << "Error: se esperaba 'end' al final de la declaración." << endl;
            exit(1);
        }
        s = new IfStatement(e, tb, fb);

    }
    // else if (match(Token::)) {
    //     e = parseCExp();
    //     if (!match(Token::DO)) {
    //         cout << "Error: se esperaba 'do' después de la expresión." << endl;
    //         exit(1);
    //     }
    //     tb = parseBody();
    //     if (!match(Token::ENDWHILE)) {
    //         cout << "Error: se esperaba 'endwhile' al final de la declaración." << endl;
    //         exit(1);
    //     }
    //     s = new WhileStatement(e, tb);

    else if(match(Token::FOR)){
        if(!match(Token::PI)){
            cout << "Error: se esperaba '(' después de 'for'." << endl;
            exit(1);
        }
        if (!match(Token::ID)) {
            cout << "Error: se esperaba id después." << endl;
            exit(1);
        }
        string temporal_variable = previous->text;
        if (!match(Token::IN)) {
            cout << "Error: se esperaba 'in'" << endl;
            exit(1);
        }
        Exp* start = parseCExp();
        if (!match(Token::DDOT)) {
            cout << "Error: se esperaba '..'" << endl;
            exit(1);
        }
        Exp* end = parseCExp();

        if (!match(Token::PD)) {
            cout << "Error: se esperaba ')' después de la expresión." << endl;
            exit(1);
        }
        if (!match(Token::LEFT_BRACKETS)) {
            cout << "Error: se esperaba '{' al final de la declaración." << endl;
            exit(1);
        }
        //VarDec* temporal_variable_ = new VarDec("Int", {temporal_variable}, "var");
        tb = parseBody();
        // save the iterator in the body
        // auto iterator_assign = new AssignStatement(temporal_variable, start);
        // tb->slist->stms.push_front(iterator_assign);
        if (!match(Token::RIGHT_BRACKETS)) {
            cout << "Error: se esperaba '}' al final de la declaración." << endl;
            exit(1);
        }
        Exp *step = new NumberExp(1);
        s = new ForStatement(start, end, step, tb, temporal_variable);
    }
    else if(match(Token::RETURN)){
        e = parseCExp();
        s = new ReturnStatement(e); //Si es null, no hay problema
    }
    else {
        cout << "Error: Se esperaba un identificador o 'print', pero se encontró: GG" << *current << endl;
        exit(1);
    }
    return s;
}

Exp* Parser::parseCExp(){
    Exp* left = parseExpression();
    if (match(Token::GREATER_THAN) ){
        BinaryOp op;
        if (previous->type == Token::GREATER_THAN){
            op = GREATER_THAN_OP;
        }
        Exp* right = parseExpression();
        left = new BinaryExp(left, right, op);
    }
    return left;
}

Exp* Parser::parseExpression() {
    Exp* left = parseTerm();
    while (match(Token::PLUS) || match(Token::MINUS)) {
        BinaryOp op;
        if (previous->type == Token::PLUS){
            op = PLUS_OP;
        }
        else if (previous->type == Token::MINUS){
            op = MINUS_OP;
        }
        Exp* right = parseTerm();
        left = new BinaryExp(left, right, op);
    }
    return left;
}

Exp* Parser::parseTerm() {
    Exp* left = parseFactor();
    while (match(Token::MUL) || match(Token::DIV)) {
        BinaryOp op;
        if (previous->type == Token::MUL){
            op = MUL_OP;
        }
        else if (previous->type == Token::DIV){
            op = DIV_OP;
        }
        Exp* right = parseFactor();
        left = new BinaryExp(left, right, op);
    }
    return left;
}

Exp* Parser::parseFactor() {
    Exp* e;
    Exp* e1;
    Exp* e2;
    // if (match(Token::TRUE)){
    //     return new BoolExp(1);
    // }else if (match(Token::FALSE)){
    //     return new BoolExp(0);
    // }
     if (match(Token::NUM)) {
        return new NumberExp(stoi(previous->text));
    }
    else if (match(Token::ID)) {
        string texto = previous->text;
        //Parse FCallExp
        if (match(Token::PI)){
            list<Exp*> args;
            if (!check(Token::PD)){
                args.push_back(parseCExp());
                while (match(Token::COMA)){
                    args.push_back(parseCExp());
                }
            }
            if (!match(Token::PD)){
                cout << "Falta paréntesis derecho" << endl;
                exit(0);
            }
            return new FCallExp(texto, args);
        }
        else{
            return new IdentifierExp(previous->text);
        }

    }
    else if (match(Token::PI)){
        e = parseCExp();
        if (!match(Token::PD)){
            cout << "Falta paréntesis derecho" << endl;
            exit(0);
        }
        return e;
    }
    cout << "Error: se esperaba un número o identificador." << endl;
    exit(0);
}

