#ifndef SCANNER_HPP
#define SCANNER_HPP

#include <string>
#include "token.hpp"
#include "TokenTrie.hpp"

class Scanner
{
private:
    std::string input;
    int first, current;
    std::string prevtext;
    TokenTrie *trie;

    void skipCommentsAndGarbage();

public:
    Scanner(const char *in_s);
    Token *nextToken();
    void reset();
    ~Scanner();
    // a
    Token *lookahead(int steps);
};

void test_scanner(Scanner *scanner);

#endif // SCANNER_HPP