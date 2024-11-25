#include "TokenTrie.hpp"

TokenTrie::Node::Node() {}

TokenTrie::Node::Node(const char c, Token::Type t) : _c(c), _tokenType(t) {}

TokenTrie::Node::~Node() {}

auto TokenTrie::Node::getChar() const -> char
{
    return _c;
}

auto TokenTrie::Node::isEnd() const -> bool
{
    return _tokenType != Token::ERROR;
}

auto TokenTrie::Node::getTokenType() const -> Token::Type
{
    return _tokenType;
}

auto TokenTrie::Node::getFollow() const -> std::unordered_map<char, Node *>
{
    return _follow;
}

void TokenTrie::Node::addToFollow(const char c)
{
    _follow[c] = new Node(c);
}

void TokenTrie::Node::setType(Token::Type t)
{
    _tokenType = t;
}

TokenTrie::TokenTrie() {}

TokenTrie::TokenTrie(std::initializer_list<std::pair<std::string, Token::Type>> init)
{
    root = new Node();
    for (auto &pair : init)
    {
        Node *current = root;
        for (const auto &c : pair.first) // foreach char in token
        {
            if (current->getFollow().find(c) != current->getFollow().end()) // try to find path
            {
                current = current->getFollow()[c]; // go to that path
            }
            else // path not found, then create node
            {
                current->addToFollow(c);
                current = current->getFollow()[c];
            }
        }
        current->setType(pair.second); // set token type
    }
}

TokenTrie::TokenTrie(std::unordered_map<std::string, Token::Type> um)
{
    root = new Node();
    for (auto &pair : um)
    {
        Node *current = root;
        for (const auto &c : pair.first) // foreach char in token
        {
            if (current->getFollow().find(c) != current->getFollow().end()) // try to find path
            {
                current = current->getFollow()[c]; // go to that path
            }
            else // path not found, then create node
            {
                current->addToFollow(c);
                current = current->getFollow()[c];
            }
        }
        current->setType(pair.second); // set token type
    }
}

TokenTrie::TokenTrie(std::map<std::string, Token::Type> m)
{
    root = new Node();
    for (auto &pair : m)
    {
        Node *current = root;
        for (const auto &c : pair.first) // foreach char in token
        {
            if (current->getFollow().find(c) != current->getFollow().end()) // try to find path
            {
                current = current->getFollow()[c]; // go to that path
            }
            else // path not found, then create node
            {
                current->addToFollow(c);
                current = current->getFollow()[c];
            }
        }
        current->setType(pair.second); // set token type
    }
}

TokenTrie::~TokenTrie()
{
    delete root;
}

auto TokenTrie::getRoot() const -> TokenTrie::Node *
{
    return root;
}

auto TokenTrie::findToken(const std::string &token) const -> Token::Type
{
    Node *current = root;
    for (const auto &c : token)
    {
        if (current->getFollow().find(c) != current->getFollow().end())
        {
            current = current->getFollow()[c];
        }
        else
        {
            return Token::ERROR;
        }
    }
    return current->getTokenType();
}