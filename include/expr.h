#ifndef EXPR_H
#define EXPR_H

#include <iostream>
#include <map>
#include <string>
#include <vector>
using namespace std;

extern map<char, string> oprName;

class Expr {
   private:
    static unsigned int tempIdNum;
    string tempName;

   public:
    Expr();
    Expr(string tempName);
    string tempNameGet() const;
    virtual void setRight(Expr *ri);
    virtual string codeGen() = 0;
    virtual void debug() = 0;
    // virtual ~Expr(){}
};

class NumExpr : public Expr {
   public:
    NumExpr(string value) : Expr(value) {}
    virtual string codeGen() { return ""; }
    void debug() { cerr << "numVal\n"; }
};

class VarExpr : public Expr {
   private:
    static unsigned int varIdNum;
    static map<string, unsigned int> varMap;
    string name;
    string varName;
    void constructor() {
        map<string, unsigned int>::iterator it = varMap.lower_bound(name);
        if (it == varMap.end() || name != it->first) {
            varMap.insert(it, {name, varIdNum++});
        }
        varName = "%var" + to_string(varMap[name]);
    }

   protected:
    VarExpr(string name, void *reservedForInheritance) : Expr(""), name(name) {
        constructor();
    }

   public:
    virtual void debug() { cerr << "varVal"; }
    static vector<string> getVarList() {
        vector<string> res;
        map<string, unsigned int>::iterator it;
        for (it = varMap.begin(); it != varMap.end(); it++) {
            res.push_back("%var" + to_string(it->second));
        }
        return res;
    }
    VarExpr(string name);
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
    void debug() {
        cerr << "oprVal\n";
        cerr << "le-> ";
        le->debug();
        cerr << "ri-> ";
        ri->debug();

    }
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
    void debug() {
        cerr << "asgVal\n";
        cerr << "assignee-> ";
        assignee->debug();
    }
    AsgExpr(string name, Expr *assignee)
        : VarExpr(name, NULL), assignee(assignee) {}
    virtual string codeGen() {
        return assignee->codeGen() + "store i32 " + assignee->tempNameGet() +
               ", i32* " + varNameGet() + '\n';
    }
};
#endif