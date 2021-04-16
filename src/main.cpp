#include <cctype>
#include <climits>
#include <iostream>
#include <map>
#include <vector>

#include "../include/expr.h"
#include "../include/fileio.h"
#include "../include/generator.h"
using namespace std;

int linenum = 0;

struct InvalidExpr : public exception {
    const char *what() const throw() { return "Invalid Expression: "; }
};

enum block_type {
    num = -1,
    var = -2,
    _while = -3,
    _if = -4,
    _choose = -5,
    _print = -6,
    asg = -7,
    eof = -8
};

string lex_str;
int pos = 0;

int lex_tok(const char *text) {
    lex_str = "";

    if (text[pos] == '\0') {
        return eof;
    }

    while (isspace(text[pos]) && text[pos] != '\n') pos++;

    // cerr << "pos: " << pos << endl;
    if (text[pos] == '\0') {
        return eof;
    }

    if (text[pos] == '#' || text[pos] == '\n') {
        // cerr << "char: " << text[pos] << endl;
        while (text[pos] != '\n' && text[pos] != '\0') pos++;
        if (text[pos] == '\0') {
            return eof;
        }

        pos++;
        // cerr << "ret: " << 1 << endl;
        return '\n';
    }

    if (isdigit(text[pos])) {
        do {
            lex_str += text[pos++];
        } while (isdigit(text[pos]));
        // cerr << "ret: " << 2 << endl;
        return num;
    }
    if (isalpha(text[pos])) {
        do {
            lex_str += text[pos++];
        } while (isalnum(text[pos]));

        // Clear whitespace to check predefined definitions.
        // Whitespace would be cleared on next lexer call anyway.
        while (isspace(text[pos]) && text[pos] != '\n' && text[pos] != '\0')
            pos++;
        if (text[pos] == '\0' || text[pos] == '\n') {
            return var;
        }

        // If predefined definition-function, return type.
        if (text[pos] == '(') {
            pos++;
            if (lex_str == "if") return _if;
            if (lex_str == "while") return _while;
            if (lex_str == "print") return _print;
            if (lex_str == "choose") return _choose;
            cerr << "q: " << lex_str << endl;
            throw InvalidExpr();  // Undefined function call
        }
        // If assign operator, return assign token.
        // Variable name to be set is in lex_str.
        if (text[pos] == '=') {
            pos++;
            // cerr << "ret: " << 3 << endl;
            return asg;
        }

        // cerr << "ret: " << 4 << endl;
        return var;
    }

    // cerr << "ret: " << 5 << endl;
    return text[pos++];
}

map<char, int> oprPrec = {
    {'+', 1},
    {'-', 1},
    {'*', 2},
    {'/', 2},
};

enum endChar { newline = '\n', paran = ')', comma = ',' };

vector<Expr *> parseArgs(const char *text, int argNum);

Expr *parseExpr(const char *text, endChar endExpr = newline) {
    // The first part should always be on leaves.
    cerr << "Im here\n";
    int fi_tok = lex_tok(text), se_tok, last_prec, cur_prec = INT_MAX;
    cerr << fi_tok << endl;
    string cur_str = lex_str, next_str;
    Expr *top, *cur, *part, *temp;
    cur = new NumExpr("0");
    switch (fi_tok) {
        case '(':
            top = parseExpr(text, paran);
            break;
        case var:
            top = new VarExpr(cur_str);
            break;
        case num:
            top = new NumExpr(cur_str);
            break;
        case _choose:
            top = new FuncExpr("choose", parseArgs(text, 4));
            break;
        /*case asg:
            return new AsgExpr(cur_str, parseExpr(text, endExpr));*/
        default:
            cerr << "1: " << fi_tok << endl;
            throw InvalidExpr();  // Invalid Expression
    }
    cerr << "cur_str_beg: " << cur_str << endl;
    se_tok = lex_tok(text);
    cerr << se_tok << endl;
    if (oprPrec.find(se_tok) == oprPrec.end()) {
        if ((se_tok == endExpr) || (endExpr == newline && se_tok == eof)) {
            return top;
        }
        cerr << "3-1: " << (char)fi_tok << endl;
        throw InvalidExpr();
    }
    while (oprPrec.find(se_tok) != oprPrec.end()) {
        cerr << oprPrec.find(se_tok)->first;
        last_prec = cur_prec;
        cur_prec = oprPrec[se_tok];
        if (cur_prec < last_prec) {
            cerr << "low\n";
            // cur->debug();
            cur->setRight(part);
            top = new OprExpr(se_tok, top);
            cur = top;
        } else {
            cerr << "high\n";
            temp = new OprExpr(se_tok, part);
            cur->setRight(temp);
            cur = temp;
        }
        fi_tok = lex_tok(text);
        cur_str = lex_str;
        cerr << "cur_str: " << cur_str << endl;
        cerr << "fi_tok: " << fi_tok << endl;
        switch (fi_tok) {
            case '(':
                part = parseExpr(text, paran);
                break;
            case var:
                part = new VarExpr(cur_str);
                break;
            case num:
                part = new NumExpr(cur_str);
                break;
            case _choose:
                part = new FuncExpr("choose", parseArgs(text, 4));
                break;
            default:
                cerr << "2: " << fi_tok << endl;
                throw InvalidExpr();  // Invalid Expression
        }
        se_tok = lex_tok(text);
        cerr << "se_tok: " << se_tok << endl;
    }
    cur->setRight(part);
    if (se_tok == endExpr || (endExpr == newline && se_tok == eof)) {
        return top;
    }
    cerr << "3: " << (char)fi_tok << endl;
    throw InvalidExpr();
}

vector<Expr *> parseArgs(const char *text, int argNum) {
    vector<Expr *> res;
    while (argNum > 1) {
        res.push_back(parseExpr(text, comma));
        argNum--;
    }
    if (argNum == 1) res.push_back(parseExpr(text, paran));
    cerr << "Returning\n" << res.size() << " "<< pos << endl;
    return res;
}

string generatePrint(Expr *tgt) {
    return "call i32 (i8*, ...)* @printf(i8* getelementptr ([4 x i8]* "
           "@print.str, i32 0, i32 0), i32 " +
           tgt->tempNameGet() + " )\n";
}

int whileNum = 1;
int ifNum = 1;

void runWhile(Expr *tgt, string &text, Generator &gen) {
    string whileName[] = {"whcond" + to_string(whileNum),
                          "whbody" + to_string(whileNum),
                          "whend" + to_string(whileNum)};
    whileNum++;
    string cmpTemp = Expr::tempNameRequest();

    gen.add_code(tgt->codeGen());

    gen.add_code(whileName[0] + ":\n" + cmpTemp + " = icmp ne i32 " +
                 tgt->tempNameGet() + ", 0\nbr i1 " + tgt->tempNameGet() +
                 ", label " + whileName[1] + ", label " + whileName[2] + '\n' +
                 whileName[1] + ":\n");
    cerr << "While parsed";
    linenum++;
    int tok = lex_tok(text.c_str());
    string cur_str = lex_str;
    Expr *exprPtr;
    while (tok != '}') {
        // cerr << "tok: " << tok << endl;
        cerr << "str: " << text[pos] << text[pos + 1] << text[pos + 2] << endl;
        switch (tok) {
            case asg:
                exprPtr = new AsgExpr(cur_str, parseExpr(text.c_str()));
                cerr << "pos: " << pos << endl;
                cerr << "Hello\n";
                // exprPtr->debug();
                gen.add_code(exprPtr->codeGen());
                break;
            case _print:
                exprPtr = parseArgs(text.c_str(), 1)[0];
                gen.add_code(exprPtr->codeGen());
                gen.add_code(generatePrint(exprPtr));
                break;
            case '\n':
                break;
            default:
                cerr << "def: " << tok << endl;
                throw InvalidExpr();  // Invalid Expression
        }
        cerr << "ok-while: " << linenum << ": " << tok << ": " << pos << endl;
        tok = lex_tok(text.c_str());
        if (tok == eof) {
            throw InvalidExpr();  // Invalid Expression
        }
        cur_str = lex_str;
        linenum++;
    }

    gen.add_code("br label " + whileName[0] + '\n' + whileName[2] + ":\n");
}

void runIf(Expr *tgt, string &text, Generator &gen) {
    string ifName[] = {"ifcond" + to_string(ifNum), "ifbody" + to_string(ifNum),
                       "ifend" + to_string(ifNum)};
    ifNum++;
    string cmpTemp = Expr::tempNameRequest();

    gen.add_code(tgt->codeGen());

    gen.add_code(ifName[0] + ":\n" + cmpTemp + " = icmp ne i32 " +
                 tgt->tempNameGet() + ", 0\nbr i1 " + tgt->tempNameGet() +
                 ", label " + ifName[1] + ", label " + ifName[2] + '\n' +
                 ifName[1] + ":\n");
    cerr << "If parsed";
    linenum++;
    int tok = lex_tok(text.c_str());
    string cur_str = lex_str;
    Expr *exprPtr;
    while (tok != '}') {
        // cerr << "tok: " << tok << endl;
        cerr << "str: " << text[pos] << text[pos + 1] << text[pos + 2] << endl;
        switch (tok) {
            case asg:
                exprPtr = new AsgExpr(cur_str, parseExpr(text.c_str()));
                cerr << "pos: " << pos << endl;
                cerr << "Hello\n";
                // exprPtr->debug();
                gen.add_code(exprPtr->codeGen());
                break;
            case _print:
                exprPtr = parseArgs(text.c_str(), 1)[0];
                gen.add_code(exprPtr->codeGen());
                gen.add_code(generatePrint(exprPtr));
                break;
            case '\n':
                break;
            default:
                cerr << "def: " << tok << endl;
                throw InvalidExpr();  // Invalid Expression
        }
        cerr << "ok-if: " << linenum << ": " << tok << ": " << pos << endl;
        tok = lex_tok(text.c_str());
        if (tok == eof) {
            throw InvalidExpr();  // Invalid Expression
        }
        cur_str = lex_str;
        linenum++;
    }

    gen.add_code(ifName[2] + ":\n");
}

int main(int argc, char *argv[]) {
    ios::sync_with_stdio(false);

    if (argc < 2) {
        cerr << "Please enter a filename!\n";
        return 0;
    }

    try {
        FileIO io(argv[1]);

        string text = io.readFile() + "\0";
        Generator gen = Generator();
        int tok = lex_tok(text.c_str());
        string cur_str = lex_str;
        Expr *exprPtr;
        while (tok != eof) {
            // cerr << "tok: " << tok << endl;
            cerr << "str: " << text[pos] << text[pos + 1] << text[pos + 2]
                 << endl;
            switch (tok) {
                case asg:
                    exprPtr = new AsgExpr(cur_str, parseExpr(text.c_str()));
                    cerr << "pos: " << pos << endl;
                    cerr << "Hello\n";
                    // exprPtr->debug();
                    gen.add_code(exprPtr->codeGen());
                    break;
                case _print:
                    exprPtr = parseArgs(text.c_str(), 1)[0];
                    gen.add_code(exprPtr->codeGen());
                    gen.add_code(generatePrint(exprPtr));
                    break;
                case _while:
                    exprPtr = parseArgs(text.c_str(), 1)[0];
                    if (lex_tok(text.c_str()) != '{') {
                        cerr << '{';
                        throw InvalidExpr();  // Invalid Expression
                    }
                    if (lex_tok(text.c_str()) != '\n') {
                        cerr << 'n';
                        throw InvalidExpr();  // Invalid Expression
                    }
                    cerr << "success";
                    runWhile(exprPtr, text, gen);
                    break;

                case _if:
                    exprPtr = parseArgs(text.c_str(), 1)[0];
                    if (lex_tok(text.c_str()) != '{') {
                        cerr << '{';
                        throw InvalidExpr();  // Invalid Expression
                    }
                    if (lex_tok(text.c_str()) != '\n') {
                        cerr << 'n';
                        throw InvalidExpr();  // Invalid Expression
                    }
                    cerr << "success";
                    runIf(exprPtr, text, gen);
                    break;

                case '\n':
                    break;
                default:
                    cerr << "def: " << tok << endl;
                    throw InvalidExpr();  // Invalid Expression
            }
            cerr << "ok: " << linenum << ": " << tok << ": " << pos << endl;
            tok = lex_tok(text.c_str());
            cur_str = lex_str;
            linenum++;
        }
        cerr << "ok" << endl;
        for (auto it : VarExpr::getVarList()) {
            gen.add_init(it);
        }
        cerr << "ok" << endl;
        io.writeFile(gen.get_code());
        cerr << "done" << endl;

    } catch (const exception &e) {
        // Write the type of exception, then description.
        // Must be updated before submitting the assignment.
        exception_ptr p = make_exception_ptr(e);
        cerr << (p ? p.__cxa_exception_type()->name() : "null") << endl
             << e.what() << endl;
        cerr << "Line Number: " << linenum << endl;
    }

    return 0;
}