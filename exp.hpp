#ifndef EXP_HPP
#define EXP_HPP

#include <string>
#include <unordered_map>
#include <list>
#include "visitor.hpp"
#include <vector>

using namespace std;
enum BinaryOp
{
    PLUS_OP,
    MINUS_OP,
    MUL_OP,
    DIV_OP,
    LT_OP,
    LE_OP,
    EQ_OP
};

class Body;

class Exp
{
public:
    virtual int accept(Visitor *visitor) = 0;
    virtual ~Exp() = 0;
    static string binopToChar(BinaryOp op);
};
class IFExp : public Exp
{
public:
    Exp *cond, *left, *right;
    IFExp(Exp *cond, Exp *l, Exp *r);
    int accept(Visitor *visitor);
    ~IFExp();
};

class BinaryExp : public Exp
{
public:
    Exp *left, *right;
    BinaryOp op;
    BinaryExp(Exp *l, Exp *r, BinaryOp op);
    int accept(Visitor *visitor);
    ~BinaryExp();
};

class NumberExp : public Exp
{
public:
    int value;
    NumberExp(int v);
    int accept(Visitor *visitor);
    ~NumberExp();
};

class BoolExp : public Exp
{
public:
    int value;
    BoolExp(bool v);
    int accept(Visitor *visitor);
    ~BoolExp();
};

class IdentifierExp : public Exp
{
public:
    std::string name;
    IdentifierExp(const std::string &n);
    int accept(Visitor *visitor);
    ~IdentifierExp();
};

class FCallExp : public Exp
{
public:
    string id;
    list<Exp *> arglist;
    FCallExp() {}
    FCallExp(string id) : id(id) {}
    FCallExp(string id, list<Exp *> arglist) : id(id), arglist(arglist) {}
    ~FCallExp() {}
    int accept(Visitor *visitor)
    {
        return visitor->visit(this);
    }
};

class Stm
{
public:
    virtual int accept(Visitor *visitor) = 0;
    virtual ~Stm() = 0;
};

class AssignStatement : public Stm
{
public:
    std::string id;
    Exp *rhs;
    AssignStatement(std::string id, Exp *e);
    int accept(Visitor *visitor);
    ~AssignStatement();
};

class PrintStatement : public Stm
{
public:
    Exp *e;
    PrintStatement(Exp *e);
    int accept(Visitor *visitor);
    ~PrintStatement();
};

class IfStatement : public Stm
{
public:
    Exp *condition;
    Body *then;
    Body *els;
    IfStatement(Exp *condition, Body *then, Body *els);
    int accept(Visitor *visitor);
    ~IfStatement();
};
class WhileStatement : public Stm
{
public:
    Exp *condition;
    Body *b;
    WhileStatement(Exp *condition, Body *b);
    int accept(Visitor *visitor);
    ~WhileStatement();
};

class ForStatement : public Stm
{
public:
    Exp *start;
    Exp *end;
    Exp *step;
    Body *b;
    ForStatement(Exp *start, Exp *end, Exp *step, Body *b);
    int accept(Visitor *visitor);
    ~ForStatement();
};

class VarDec
{
public:
    string type;
    list<string> vars;
    VarDec(string type, list<string> vars);
    int accept(Visitor *visitor);
    ~VarDec();
};

class VarDecList
{
public:
    list<VarDec *> vardecs;
    VarDecList();
    void add(VarDec *vardec);
    int accept(Visitor *visitor);
    ~VarDecList();
};

class StatementList
{
public:
    list<Stm *> stms;
    StatementList();
    void add(Stm *stm);
    int accept(Visitor *visitor);
    ~StatementList();
};

class Body
{
public:
    VarDecList *vardecs;
    StatementList *slist;
    Body(VarDecList *vardecs, StatementList *stms);
    int accept(Visitor *visitor);
    ~Body();
};
class ParamDecList
{
public:
    vector<string> types;
    vector<string> ids;
    int accept(Visitor *visitor)
    {
        visitor->visit(this);
        return 0;
    }
    ParamDecList() {}
    ~ParamDecList() {}
};
class FunDec
{
public:
    string type;
    string id;
    ParamDecList *param_dec_list;
    Body *body;

    FunDec() {}
    FunDec(string type, string id, ParamDecList *param_dec_list, Body *body) : type(type), id(id), param_dec_list(param_dec_list), body(body)
    {
    }
    ~FunDec() {}
    int accept(Visitor *visitor)
    {
        visitor->visit(this);
        return 0;
    }
};
class FunDecList
{
public:
    list<FunDec *> fun_decs;
    void add(FunDec *fundec)
    {
        fun_decs.push_back(fundec);
    }
    int accept(Visitor *visitor)
    {
        visitor->visit(this);
        return 0;
    }
    FunDecList() {}
    ~FunDecList() {}
};

class ReturnStatement : public Stm
{
public:
    int accept(Visitor *visitor)
    {
        visitor->visit(this);
        return 0;
    }
    Exp *exp;
    ReturnStatement() {}
    ReturnStatement(Exp *exp) : exp(exp) {}
    ~ReturnStatement() {}
};

class Program
{
public:
    VarDecList *var_dec_lists;
    FunDecList *fun_dec_lists;
    Program() {}
    Program(VarDecList *var_dec_lists, FunDecList *fun_dec_lists)
        : var_dec_lists(var_dec_lists), fun_dec_lists(fun_dec_lists) {}
    ~Program() {}
};

#endif // EXP_HPP