#include "../include/expr.h"
using namespace std;

map<char, string> oprName = {
    {'+', "add"},
    {'-', "sub"},
    {'*', "mul"},
    {'/', "sdiv"},
};

unsigned int Expr::tempIdNum = 1;
unsigned int VarExpr::varIdNum = 1;
map<string, unsigned int> VarExpr::varMap;


Expr::Expr() : tempName("%tmp" + to_string(tempIdNum++)) {}
Expr::Expr(string tempName) : tempName(tempName) {}
string Expr::tempNameGet() const { return tempName; }
string Expr::tempNameRequest() { return "%tmp" + to_string(tempIdNum++); }


// Generrate the LLVM IR Code for the variables
VarExpr::VarExpr(string name) : name(name) { constructor(); }
VarExpr::VarExpr(string name, void *reserved) : Expr(""), name(name) {
    constructor();
}

void VarExpr::constructor() {
    map<string, unsigned int>::iterator it = varMap.lower_bound(name);
    if (it == varMap.end() || name != it->first) {
        varMap.insert(it, {name, varIdNum++});
    }   
    varName = "%var" + to_string(varMap[name]);
}

vector<string> VarExpr::getVarList() {
    vector<string> res;
    map<string, unsigned int>::iterator it;
    for (it = varMap.begin(); it != varMap.end(); it++) {
        res.push_back("%var" + to_string(it->second));
    }
    return res;
}

// Generrate the LLVM IR Code for the choose functions
FuncExpr::FuncExpr(string funcname, vector<Expr *> args)
    : args(args), funcname(funcname) {}
string FuncExpr::codeGen() {
    string str_args = "", pre_code = "";
    for (int i = 0; i < args.size(); i++) {
        str_args += "i32 " + args[i]->tempNameGet();
        pre_code += args[i]->codeGen();
        if ((i + 1) < args.size()) {
            str_args += ", ";
        }
    }
    return pre_code + tempNameGet() + " = call i32 @" + funcname + '(' +
           str_args + ")\n";
}

string VarExpr::varNameGet() const { return varName; }
string VarExpr::codeGen() {
    return tempNameGet() + " = load i32* " + varName + '\n';
}

// Generrate the LLVM IR Code for the assignment expressions
AsgExpr::AsgExpr(string name, Expr *assignee)
    : VarExpr(name, NULL), assignee(assignee) {}
string AsgExpr::codeGen() {
    return assignee->codeGen() + "store i32 " + assignee->tempNameGet() +
           ", i32* " + varNameGet() + '\n';
}
AsgExpr::~AsgExpr() {
    delete assignee;
}

// Generrate the LLVM IR Code for the operator expressions
OprExpr::OprExpr(char op, Expr *le, Expr *ri) : op(op), le(le), ri(ri) {}
OprExpr::OprExpr(char op, Expr *le) : op(op), le(le) {}
void OprExpr::setRight(Expr *ri) { this->ri = ri; }
void OprExpr::pushLeft(char op, Expr *var) {
    OprExpr *temp = new OprExpr(this->op, this->le, this->ri);
    temp->ri = var;
    this->le = temp;
    this->op = op;
}
string OprExpr::codeGen() {
    string left, right;
    left = le->codeGen();
    right = ri->codeGen();
    return left + right + tempNameGet() + " = " + oprName[op] + " i32 " +
           le->tempNameGet() + ", " + ri->tempNameGet() + '\n';
}
OprExpr::~OprExpr() {
    delete le;
    delete ri;
}
