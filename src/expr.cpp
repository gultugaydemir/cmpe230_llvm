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

VarExpr::VarExpr(string name) : name(name) {
    constructor();
}

/*
class Expr {
   private:
    static unsigned int tempIdNum;
    string tempName;

   public:
    Expr() : tempName("%tmp" + to_string(tempIdNum++)) {}
    Expr(string tempName) : tempName(tempName) {}
    string tempNameGet() const { return tempName; }
    virtual void setRight(Expr *ri) {}
    virtual string codeGen() = 0;
    virtual ~Expr(){}
};

class NumExpr : public Expr {
   public:
    NumExpr(string value) : Expr(value) {}
    virtual string codeGen() { return ""; }
};

class VarExpr : public Expr {
   private:
    static unsigned int varIdNum;
    static map<string, unsigned int> varMap;
    string name;
    string varName;

   public:
    static vector<string> getVarList() {
        vector<string>res;
        map<string, unsigned int>::iterator it;
        for(it=varMap.begin(); it!= varMap.end(); it++){
            res.push_back("%var" + to_string(it->second));
        }
        return res;
    }
    VarExpr(string name) : name(name) {
        map<string, unsigned int>::iterator it = varMap.lower_bound(name);
        if (it == varMap.end() || name != it->first) {
            varMap.insert(it, {name, varIdNum++});
        }
        varName = "%var" + to_string(varMap[name]);
    }
    string varNameGet() const { return varName; }
    virtual string codeGen() {
        return tempNameGet() + " = load i32* " + varName + '\n';
    }
};

class OprExpr : public Expr {
   private:
    char op;
    Expr *le, *ri;

   public:
    OprExpr(char op, Expr *le, Expr *ri) : op(op), le(le), ri(ri) {}
    OprExpr(char op, Expr *le) : op(op), le(le) {}
    virtual void setRight(Expr *ri) { this->ri = ri; }
    virtual string codeGen() {
        string left, right;
        left = le->codeGen();
        right = ri->codeGen();
        return left + right + tempNameGet() + " = " + oprName[op] + " i32 " +
               le->tempNameGet() + ", " + ri->tempNameGet() + '\n';
    }
};

class AsgExpr : public VarExpr {
   private:
    Expr *assignee;

   public:
    AsgExpr(string name, Expr *assignee) : VarExpr(name), assignee(assignee) {}
    virtual string codeGen() {
        return assignee->codeGen() + "store i32 " + assignee->tempNameGet() +
               ", i32* " + tempNameGet() + '\n';
    }
};
*/