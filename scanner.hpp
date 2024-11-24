#ifndef SCANNER_HPP
#define SCANNER_HPP

#include <string>
#include "token.hpp"
#include "tokentrie.hpp"

class Scanner
{
private:
    std::string input;
    int first, current;
    std::string prevtext;
    TokenTrie *trie;

    int row, column;

    void skipComments();
    void addRow(int n);
    void addColumn(int n);

public:
    Scanner(const char *in_s);
    Token *nextToken();
    void reset();
    void throw_unrecognizedCharacter();
    void throw_OutOfBounds();
    std::string printPosition();
    ~Scanner();
};

void test_scanner(Scanner *scanner);

#endif // SCANNER_HPP