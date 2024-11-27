#include <iostream>
#include <cstring>
#include "scanner.hpp"
#include "TokenTrie.hpp"

using namespace std;

Scanner::Scanner(const char *s) : input(s), first(0), current(0), prevtext("")
{
    trie = new TokenTrie({{"+", Token::ADD},
                          {"-", Token::SUB},
                          {"*", Token::MUL},
                          {"/", Token::DIV},
                          {"=", Token::ASSIGN},
                          {"if", Token::IF},
                          {"else", Token::ELSE},
                          {",", Token::COMMA},
                          {"for", Token::FOR},
                          {"return", Token::RETURN},
                          {">", Token::GREATER_THAN},
                          {">=", Token::GREATER_EQUAL},
                          {"<", Token::LESS_THAN},
                          {"<=", Token::LESS_EQUAL},
                          {"!=", Token::NOT_EQUAL},
                          {"==", Token::EQUAL},
                          {"fun", Token::FUN},
                          {"endfun", Token::ENDFUN},
                          {"(", Token::LEFT_PARENTHESIS},
                          {")", Token::RIGHT_PARENTHESIS},
                          {"val", Token::VAL},
                          {"var", Token::VAR},
                          {":", Token::COLON},
                          {"println", Token::PRINTLN},
                          {"{", Token::LEFT_BRACKET},
                          {"}", Token::RIGHT_BRACKET},
                          {";", Token::SEMICOLON},
                          {"..", Token::DOUBLE_DOT},
                          {"in", Token::IN},
                          {"step", Token::STEP},
                          {"downto", Token::DOWNTO},
                          {"while", Token::WHILE},
                          {"do", Token::DO}});
}

bool isWhiteSpace(char c)
{
    return c == ' ' || c == '\r' || c == '\t';
}

void Scanner::skipCommentsAndGarbage()
{
    while (current < input.length())
    {
        // Comnetario de linea
        if (current < input.length() - 1 && input[current] == '/' && input[current + 1] == '/')
        {
            while (current < input.length() && input[current] != '\n')
            {
                current++;
            }
            continue;
        }

        if (current < input.length() - 1 && input[current] == '/' && input[current + 1] == '*')
        {
            current += 2;
            while (current < input.length() - 1)
            {
                if (input[current] == '*' && input[current + 1] == '/')
                {
                    current += 2;
                    break;
                }
                current++;
            }
            if (current >= input.length())
            {
                cout << "Error" << endl;
            }
            continue;
        }
        // Ignorar salto de linea si ya tiene token subsiguiente o es un antiguo salto de linea
        if (isWhiteSpace(input[current]) || (( // No es necesario no tener un salto de linea despues de l
                                                 prevtext == "{" || prevtext == "\n" || prevtext == ";" ||
                                                 prevtext == "+" || prevtext == "-" || prevtext == "*" || prevtext == "/" ||
                                                 prevtext == ">" || prevtext == ">=" || prevtext == "==") &&
                                             input[current] == '\n'))
        {
            current++;
        }
        else
        {
            break;
        }
    }
}

Token *Scanner::nextToken()
{
    skipCommentsAndGarbage();
    Token *token;

    if (current >= input.length())
        return new Token(Token::END);

    char c = input[current];
    first = current;

    if (input[current] == '\n')
    {
        token = new Token(Token::LINE_BREAK, "\n");
        prevtext = "\n";
        current++;
        return token;
    }

    if (c == '-' || c == '+' && current + 1 < input.length() && isdigit(input[current + 1]))
    {
        ++current;
        current++;
        while (current < input.length() && isdigit(input[current]))
            current++;
        string number = input.substr(first, current - first);
        token = new Token(Token::NUM, number);
    }
    else if (isdigit(c))
    {
        current++;
        while (current < input.length() && isdigit(input[current]))
            current++;
        string number = input.substr(first, current - first);
        token = new Token(Token::NUM, number);
    }
    else if (isalpha(c))
    {
        current++;
        while (current < input.length() && isalnum(input[current]))
            current++;
        string word = input.substr(first, current - first);
        Token::Type type = trie->findToken(word);
        token = type != Token::ERROR ? new Token(type, word) : new Token(Token::ID, word);
    }
    else
    {
        int iterator = current;
        int lastCorrectToken = current;
        string word = "";
        Token::Type type = Token::ERROR;
        while (!isWhiteSpace(input[iterator]) && !isalpha(input[iterator]) && !isdigit(input[iterator]) && iterator < input.length())
        {
            word += input[iterator];
            Token::Type found = trie->findToken(word);
            if (found != Token::ERROR)
            {
                type = found;
                lastCorrectToken = iterator + 1;
            }
            ++iterator;
        }
        word = word.substr(0, lastCorrectToken - current);
        token = new Token(type, word);
        current = lastCorrectToken;
    }
    prevtext = token->getText();
    return token;
}

void Scanner::reset()
{
    first = 0;
    current = 0;
}

Scanner::~Scanner()
{
}

void test_scanner(Scanner *scanner)
{
    Token *current;
    cout << "Iniciando Scanner:" << endl
         << endl;
    while ((current = scanner->nextToken())->getType() != Token::END)
    {
        if (current->getType() == Token::ERROR)
        {
            cout << "Error en scanner - carácter inválido: " << current->getText() << endl;
            break;
        }
        else
        {
            cout << *current << endl;
        }
        delete current;
    }
    cout << "TOKEN(END)" << endl;
    delete current;
}
