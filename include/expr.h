#ifndef EXPR_H
#define EXPR_H

#include <iostream>
#include <map>
#include <string>
#include <vector>
using namespace std;

// Base class for all types of expressions 
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
    virtual ~Expr(){};
};

// NumExpr - Expression class for numeric values
class NumExpr : public Expr {
   public:
    NumExpr(string value) : Expr(value) {}
    virtual string codeGen() { return string(); }
};


// FuncExpr - Expression class for functions
class FuncExpr : public Expr {
   private:
    vector<Expr *> args;
    string funcname;

   public:
    FuncExpr(string funcname, vector<Expr *> args);
    virtual string codeGen();
};

// VarExpr - Expression class for variables
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
    static vector<string> getVarList();
    VarExpr(string name);
    string varNameGet() const;
    virtual string codeGen();
};

// AsgExpr - Expression class for assignment operations
class AsgExpr : public VarExpr {
   private:
    Expr *assignee;

   public:
    AsgExpr(string name, Expr *assignee);
    virtual string codeGen();
    virtual ~AsgExpr();
};

// OprExpr - Expression class for operators
class OprExpr : public Expr {
   private:
    char op;
    Expr *le = NULL, *ri = NULL;

   public:
    OprExpr(char op, Expr *le, Expr *ri);
    OprExpr(char op, Expr *le);
    virtual void setRight(Expr *ri);
    virtual void pushLeft(char op, Expr *var);
    virtual string codeGen();
    virtual ~OprExpr();
};

#endif