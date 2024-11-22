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

    void skipComments();

public:
    Scanner(const char *in_s);
    Token *nextToken();
    void reset();
    ~Scanner();
};

void test_scanner(Scanner *scanner);

#endif // SCANNER_HPP