    #include <iostream>
    #include <cstring>
    #include "token.h"
    #include "scanner.h"

    using namespace std;

    Scanner::Scanner(const char* s):input(s),first(0), current(0) { }


    bool is_white_space(char c) {
        return c == ' ' ||  c == '\r' || c == '\t' ;
    }

    Token* Scanner::nextToken() {
        Token* token;
        while (current < input.length() &&  is_white_space(input[current]
        )  or (
                //Aqui colocar todos lso casos en los q salto de linea ya no es necesario
                //Pq text y no token?, [Error manejo de puntero sufri]
                (
                prevtext=="{" or
                prevtext=="\n" or
                prevtext == ";" or
                prevtext == "}" or
                //Operaciones binarias:
                prevtext == "+" or
                prevtext == "-" or
                prevtext == "*" or
                prevtext == "/" or
                prevtext == ">" or
                prevtext == ">=" or
                prevtext == "=="

                )
            and input[current] == '\n'
            ) ) {
            current++;
        }
        if (current >= input.length()) return new Token(Token::END);
        char c  = input[current];
        first = current;

        if (input[current]=='\n') {
            token = new Token(Token::LINE_BREAK, "\n");
            prevtext = "\n";
            current++;
            return token;
        }

        if (isdigit(c)) {
            current++;
            while (current < input.length() && isdigit(input[current])) {
                current++;
            }
            token = new Token(Token::NUM, input, first, current - first);
        }

        else if (isalpha(c)) {
            current++;
            while (current < input.length() && isalnum(input[current])) {
                current++;
            }
            string word = input.substr(first, current - first);
            if (word == "println") {
                token = new Token(Token::PRINTLN, word, 0, word.length());
            } else if (word == "if") {
                token = new Token(Token::IF, word, 0, word.length());
            } else if (word == "else") {
                token = new Token(Token::ELSE, word, 0, word.length());
            } else if (word == "for") {
                token = new Token(Token::FOR, word, 0, word.length());
            } else if (word == "var") {
                token = new Token(Token::VAR, word, 0, word.length());
            } else if (word == "return") {
                token = new Token(Token::RETURN, word, 0, word.length());
            }
            else if (word == "fun") {
                token = new Token(Token::FUN, word, 0, word.length());
            }
            else if (word == "endfun") {
                token = new Token(Token::ENDFUN, word, 0, word.length());
            }else if (word == "in") {
                token = new Token(Token::IN, word, 0, word.length());
            }
            else {
                token = new Token(Token::ID, word, 0, word.length());
            }
        }

        else if (strchr("+-*/()=;,>{}:.", c)) {
            switch(c) {
                case '+': token = new Token(Token::PLUS, c); break;
                case '-': token = new Token(Token::MINUS, c); break;
                case '*': token = new Token(Token::MUL, c); break;
                case '/': token = new Token(Token::DIV, c); break;
                case ',': token = new Token(Token::COMA, c); break;
                case '(': token = new Token(Token::PI, c); break;
                case ')': token = new Token(Token::PD, c); break;
                case '.':
                    if (current + 1 < input.length() && input[current + 1] == '.') {
                        token = new Token(Token::DDOT, "..", 0, 2);
                        current++;
                    }
                break;

                case '=':
                    if (current + 1 < input.length() && input[current + 1] == '=') {
                        token = new Token(Token::EQUAL, "==", 0, 2);
                        current++;
                    } else {
                        token = new Token(Token::ASSIGN, c);
                    }
                    break;
                case '>':
                    if (current + 1 < input.length() && input[current + 1] == '=') {
                        token = new Token(Token::GREATER_EQUAL, ">=", 0, 2);
                        current++;
                    } else {
                        token = new Token(Token::GREATER_THAN, c);
                    }
                    break;
                case ';': token = new Token(Token::PC, c); break;
                case '{': token = new Token(Token::LEFT_BRACKETS, c); break;
                case '}': token = new Token(Token::RIGHT_BRACKETS, c); break;
                case ':': token = new Token(Token::COLON, c); break;
                default:
                    cout << "No debería llegar acá" << endl;
                    token = new Token(Token::ERR, c);
            }
            current++;
        }
        else {
            token = new Token(Token::ERR, c);
            current++;
        }


        // if (prevToken) {
        //     delete prevToken;
        // }
        // prevToken = token;
        prevtext = token->text;
        return token;
    }

    void Scanner::reset() {
        first = 0;
        current = 0;
    }

    Scanner::~Scanner() {
    }

    void test_scanner(Scanner* scanner) {
        Token* current;
        cout << "Iniciando Scanner:" << endl<< endl;
        while ((current = scanner->nextToken())->type != Token::END) {
            if (current->type == Token::ERR) {
                cout << "Error en scanner - carácter inválido: " << current->text << endl;
                break;
            } else {
                cout << *current << endl;
            }
            delete current;
        }
        cout << "TOKEN(END)" << endl;
        delete current;
    }

