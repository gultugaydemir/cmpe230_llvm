#ifndef EXPR_H
#define EXPR_H

#include <iostream>
#include <map>
#include <string>
#include <vector>
using namespace std;

class Expr {
   private:
    static unsigned int tempIdNum;
    string tempName;

   public:
    Expr();
    Expr(string tempName);
    string tempNameGet() const;
    static string tempNameRequest();
    virtual void setRight(Expr *ri) {}
    virtual void pushLeft(char op, Expr *var) {}
    virtual string codeGen() = 0;
    virtual void debug() = 0;
    virtual ~Expr(){};
};

class NumExpr : public Expr {
   public:
    NumExpr(string value) : Expr(value) {}
    virtual string codeGen() { return string(); }
    void debug() { cerr << "numVal\n"; }
};

class FuncExpr : public Expr {
   private:
    vector<Expr *> args;
    string funcname;

   public:
    void debug() { cerr << "ChooseFunc\n"; }
    FuncExpr(string funcname, vector<Expr *> args);
    virtual string codeGen();
};

class VarExpr : public Expr {
   private:
    static unsigned int varIdNum;
    static map<string, unsigned int> varMap;
    string name;
    string varName;
    void constructor();

   protected:
    VarExpr(string name, void *reserved);

   public:
    virtual void debug() { cerr << "varVal"; }
    static vector<string> getVarList();
    VarExpr(string name);
    string varNameGet() const;
    virtual string codeGen();
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
    AsgExpr(string name, Expr *assignee);
    virtual string codeGen();
    virtual ~AsgExpr();
};

class OprExpr : public Expr {
   private:
    char op;
    Expr *le = NULL, *ri = NULL;

   public:
    void debug() {
        cerr << "oprVal\n";
        cerr << "le-> ";
        le->debug();
        cerr << "ri-> ";
        ri->debug();
    }
    OprExpr(char op, Expr *le, Expr *ri);
    OprExpr(char op, Expr *le);
    virtual void setRight(Expr *ri);
    virtual void pushLeft(char op, Expr *var);
    virtual string codeGen();
    virtual ~OprExpr();
};

#endif