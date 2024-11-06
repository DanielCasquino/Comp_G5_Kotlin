#include <iostream>
#include "exp.h"
#include "visitor.h"
#include <unordered_map>
using namespace std;

///////////////////////////////////////////////////////////////////////////////////
int BinaryExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}
int IFExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}
int NumberExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int BoolExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int IdentifierExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int AssignStatement::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int PrintStatement::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int IfStatement::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int WhileStatement::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}
int ForStatement::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int VarDec::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}

int VarDecList::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}
int StatementList::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}
int Body::accept(Visitor* visitor) {
    visitor->visit(this);
    return 0;
}



///////////////////////////////////////////////////////////////////////////////////

int PrintVisitor::visit(BinaryExp* exp) {
    exp->left->accept(this);
    cout << ' ' << Exp::binopToChar(exp->op) << ' ';
    exp->right->accept(this);
    return 0;
}

int PrintVisitor::visit(NumberExp* exp) {
    cout << exp->value;
    return 0;
}

int PrintVisitor::visit(BoolExp* exp) {
    if(exp->value) cout << "true";
    else cout << "false";
    return 0;
}

int PrintVisitor::visit(IdentifierExp* exp) {
    cout << exp->name;
    return 0;
}

void PrintVisitor::visit(AssignStatement* stm) {
    cout << stm->id << " = ";
    stm->rhs->accept(this);
    cout << ";";
}

void PrintVisitor::visit(PrintStatement* stm) {
    cout << "print(";
    stm->e->accept(this);
    cout << ");";
}

void PrintVisitor::visit(IfStatement* stm) {
    cout << "if ";
    stm->condition->accept(this);
    cout << " then" << endl;
    stm->then->accept(this);
    if(stm->els){
        printIndent();
        cout << "else" << endl;
        stm->els->accept(this);
    }
    printIndent();
    cout << "endif";
}

void PrintVisitor::imprimir(Program* program){
    program->var_dec_lists->accept(this);
    program->fun_dec_lists->accept(this);
}



int PrintVisitor::visit(IFExp* pepito) {
    cout<< "ifexp(";
    pepito->cond->accept(this);
    cout<< ",";
    pepito->left->accept(this);
    cout<< ",";
    pepito->right->accept(this);
    cout << ")" ;
    return 0;
}

void PrintVisitor::visit(WhileStatement* stm){
    cout << "while ";
    stm->condition->accept(this);
    cout << " do" << endl;
    stm->b->accept(this);
    printIndent();
    cout << "endwhile";
}

void PrintVisitor::visit(ForStatement* stm){
    cout << "for ";
    stm->start->accept(this);
    cout << " to ";
    stm->end->accept(this);
    cout << " step ";
    stm->step->accept(this);
    cout << " do" << endl;
    stm->b->accept(this);
    cout << "endfor";
}

void PrintVisitor::visit(VarDec* stm){
    cout << "var ";
    cout << stm->type;
    cout << " ";
    for(auto i: stm->vars){
        cout << i;
        if(i != stm->vars.back()) cout << ", ";
    }
    cout << ";";
}

void PrintVisitor::visit(VarDecList* stm){
    for(auto i: stm->vardecs){
        printIndent();
        i->accept(this);
        cout << endl;
    }
}

void PrintVisitor::visit(StatementList* stm){
    for(auto i: stm->stms){
        printIndent();
        i->accept(this);
        cout << endl;
    }
}

void PrintVisitor::visit(Body* stm){
    increaseIndent();
    stm->vardecs->accept(this);
    stm->slist->accept(this);

    decreaseIndent();
}



void PrintVisitor::printIndent() {
    for (int i = 0; i < indentLevel*2; i++) {
    cout << ' ';
    }

}

///////////////////////////////////////////////////////////////////////////////////

int PrintVisitor::visit(FCallExp* exp) {
    cout<<exp->id<<"";
    cout<<"(";
    for (auto i: exp->arglist) {
        i->accept(this);
    }
    cout<<")";
    return 0;
}

void PrintVisitor::visit(Program* p) {
    visit(p);
    // p->var_dec_lists->accept(this);
    // p->fun_dec_lists->accept(this);

}

void PrintVisitor::visit(ParamDecList* p) {
    // cout<<"es---"<<p->types.size()<<endl;
    for (int i =0; i < p->types.size(); i++) {
        cout<<p->types[i]<<" ";
        cout<<p->ids[i];
    }

}

void PrintVisitor::visit(FunDec* p) {
    cout<<"fun ";
    cout<<p->type<<" ";
    cout<<p->id;
    cout<<"(";
    p->param_dec_list->accept(this);
    cout<<")"<<endl;
    p->body->accept(this);
    cout<<"endfun"<<endl;

}

void PrintVisitor::visit(FunDecList* p) {
    for (auto item: p->fun_decs) {
        printIndent();
        item->accept(this);
        cout<<endl;
    }

}

void PrintVisitor::visit(ReturnStatement* p) {
    cout<<"return (";
    if (p->exp!=nullptr) {
        p->exp->accept(this);

    }
    cout<<")";
}
