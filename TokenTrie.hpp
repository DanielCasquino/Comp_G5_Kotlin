#include <unordered_map>
#include <map>

#include "Token.hpp"

class TokenTrie
{
private:
    class Node
    {
    private:
        char _c = 0;
        Token::Type _tokenType = Token::Type::ERROR;
        std::unordered_map<char, Node *> _follow;

    public:
        Node();
        Node(const char c, Token::Type tokenType = Token::Type::ERROR);
        ~Node();
        auto getChar() const -> char;
        auto isEnd() const -> bool;
        auto getTokenType() const -> Token::Type;
        auto getFollow() const -> std::unordered_map<char, Node *>;
        void addToFollow(const char c);
        void setType(Token::Type t);
    };
    Node *root;

public:
    TokenTrie();
    TokenTrie(std::initializer_list<std::pair<std::string, Token::Type>> init);
    TokenTrie(std::unordered_map<std::string, Token::Type> um);
    TokenTrie(std::map<std::string, Token::Type> m);
    ~TokenTrie();
    auto getRoot() const -> Node *;
    auto findToken(const std::string &token) const -> Token::Type;
};