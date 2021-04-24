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

// Used for assigning tokens for specific inputs
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


/*  @desc Reads the input and returns the assigned token for each element
    @parameter text - The input of file.my
    @return Return the token of the input from block_type.
            Otherwise return end line or throw invalid expression exception.
*/
int lex_tok(const char *text) {
    lex_str = "";

    // Reached the end of the file.
    if (text[pos] == '\0') {  
        return eof;
    }

    // Skips the whitespaces or jumps to the next line.
    while (isspace(text[pos]) && text[pos] != '\n') pos++;

    // Reached the end of the file.
    if (text[pos] == '\0') {
        return eof;
    }

    // Skips the characters after # until the end of the line.
    if (text[pos] == '#' || text[pos] == '\n') {
        while (text[pos] != '\n' && text[pos] != '\0') pos++;
        if (text[pos] == '\0') {
            return eof;
        }

        pos++;
        return '\n';
    }

    // Stores the numeric value by concatenating the consecutive digits in lex_str
    if (isdigit(text[pos])) {
        do {
            lex_str += text[pos++];
        } while (isdigit(text[pos]));
        return num;
    }

    // Checks the non-numeric characters
    if (isalpha(text[pos])) {
        // Store variable name in lex_str if the consecutive characters are alphanumeric
        do {
            lex_str += text[pos++];
        } while (isalnum(text[pos]));

        // Clear whitespace to check predefined definitions.
        // Whitespace would be cleared on next lexer call anyway.
        while (isspace(text[pos]) && text[pos] != '\n' && text[pos] != '\0')
            pos++;

        // If predefined definition-function, return their tokens accordingly.
        if (lex_str == "if") {
            if (text[pos] == '(') {
                pos++;
                return _if;
            }
            throw InvalidExpr();  // Undefined function call
        }
        if (lex_str == "while") {
            if (text[pos] == '(') {
                pos++;
                return _while;
            }
            throw InvalidExpr();  // Undefined function call
        }
        if (lex_str == "choose") {
            if (text[pos] == '(') {
                pos++;
                return _choose;
            }
            throw InvalidExpr();  // Undefined function call
        }
        if (lex_str == "print") {
            if (text[pos] == '(') {
                pos++;
                return _print;
            }
            throw InvalidExpr();  // Undefined function call
        }

        // If assign operator, returns assign token.
        // Variable name to be set is in lex_str.
        if (text[pos] == '=') {
            pos++;
            return asg;
        }

        return var;
    }

    return text[pos++];
}

//Operators with their precedence values
map<char, int> oprPrec = {
    {'+', 1},
    {'-', 1},
    {'*', 2},
    {'/', 2},
};

enum endChar { newline = '\n', paran = ')', comma = ',' };

vector<Expr *> parseArgs(const char *text, int argNum);


/*  @desc parseExpr - Parses and reads the token throughout the input text, until the endExpr.
    @parameter text to be parsed, endExpr to detect until when.
    @return the AST nodes according to the input or throws invalid exception.
*/
Expr *parseExpr(const char *text, endChar endExpr = newline) {
    // The first part should always be on leaves.
    int fi_tok = lex_tok(text), se_tok, last_prec, cur_prec = INT_MAX;
    string cur_str = lex_str, next_str;
    Expr *top, *cur = NULL, *part;

    // Detecting the token of the character
    switch (fi_tok) {
        // If encountered with "(", parses until the neares ")".
        case '(':
            top = parseExpr(text, paran);
            break;
        // If encountered with a variable, calls its expression function.
        case var:
            top = new VarExpr(cur_str);
            break;
        // If encountered with a numeric expression, calls its expression function.
        case num:
            top = new NumExpr(cur_str);
            break;
        // If encountered with a "choose" function, parses and reads the next 4 expression.
        case _choose:
            top = new FuncExpr("choose", parseArgs(text, 4));
            break;
        // Invalid Expression
        default:
            throw InvalidExpr();  
    }
    se_tok = lex_tok(text);

    // If the expression doesn't contain any operator, return its token.
    if (oprPrec.find(se_tok) == oprPrec.end()) {
        if ((se_tok == endExpr) || (endExpr == newline && se_tok == eof)) {
            return top;
        }
        throw InvalidExpr();
    }

    // If the expression contains an operator, perform the operation.
    while (oprPrec.find(se_tok) != oprPrec.end()) {
        last_prec = cur_prec; // precedence identifier
        cur_prec = oprPrec[se_tok];

        // Arranging the precedence of the operations
        if (cur_prec < last_prec){   // low
            if (cur != NULL) cur->setRight(part);
            top = new OprExpr(se_tok, top);
            cur = top;
        } else if (cur_prec == last_prec) { // medium
            cur->pushLeft(se_tok, part);
        } else { // high
            Expr *temp = new OprExpr(se_tok, part);
            cur->setRight(temp);
            cur = temp;
        }

        fi_tok = lex_tok(text);
        cur_str = lex_str;

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
                throw InvalidExpr();  // Invalid Expression
        }
        se_tok = lex_tok(text);
    }

    cur->setRight(part);
    if (se_tok == endExpr || (endExpr == newline && se_tok == eof)) {
        return top;
    }
    throw InvalidExpr();
}

// @return parses according to the given argNum
vector<Expr *> parseArgs(const char *text, int argNum) {
    vector<Expr *> res;
    while (argNum > 1) {
        res.push_back(parseExpr(text, comma));
        argNum--;
    }
    if (argNum == 1) res.push_back(parseExpr(text, paran));
    return res;
}

// @return Generates IR code for print function.
string generatePrint(Expr *tgt) {
    return "call i32 (i8*, ...)* @printf(i8* getelementptr ([4 x i8]* "
           "@print.str, i32 0, i32 0), i32 " +
           tgt->tempNameGet() + " )\n";
}

int whileNum = 1;
int ifNum = 1;

// @return Parsing through the while function and generating its LLVM IR code 
void runWhile(Expr *tgt, string &text, Generator &gen) {
    
    string whileName[] = {"whcond" + to_string(whileNum),
                          "whbody" + to_string(whileNum),
                          "whend" + to_string(whileNum)};
    whileNum++;
    string cmpTemp = Expr::tempNameRequest();

    gen.add_code("br label %" + whileName[0] + "\n" + whileName[0] + ":\n");

    gen.add_code(tgt->codeGen());

    gen.add_code(cmpTemp + " = icmp ne i32 " + tgt->tempNameGet() +
                 ", 0\nbr i1 " + cmpTemp + ", label %" + whileName[1] +
                 ", label %" + whileName[2] + '\n' + whileName[1] + ":\n");
    linenum++;
    int tok = lex_tok(text.c_str());
    string cur_str = lex_str;
    Expr *exprPtr;
    while (tok != '}') {
        switch (tok) {
            case asg:
                exprPtr = new AsgExpr(cur_str, parseExpr(text.c_str()));
                gen.add_code(exprPtr->codeGen());
                delete exprPtr;
                break;
            case _print:
                exprPtr = parseArgs(text.c_str(), 1)[0];
                gen.add_code(exprPtr->codeGen());
                gen.add_code(generatePrint(exprPtr));
                delete exprPtr;
                break;
            case '\n':
                break;
            default:
                throw InvalidExpr();  // Invalid Expression
        }
        tok = lex_tok(text.c_str());
        if (tok == eof) {
            throw InvalidExpr();  // Invalid Expression
        }
        cur_str = lex_str;
        linenum++;
    }

    gen.add_code("br label %" + whileName[0] + '\n' + whileName[2] + ":\n");
}


// @return Parsing through the if function and generating its LLVM IR code
void runIf(Expr *tgt, string &text, Generator &gen) {
    string ifName[] = {"ifcond" + to_string(ifNum), "ifbody" + to_string(ifNum),
                       "ifend" + to_string(ifNum)};
    ifNum++;
    string cmpTemp = Expr::tempNameRequest();

    gen.add_code("br label %" + ifName[0] + "\n" + ifName[0] + ":\n");

    gen.add_code(tgt->codeGen());

    gen.add_code(cmpTemp + " = icmp ne i32 " + tgt->tempNameGet() +
                 ", 0\nbr i1 " + cmpTemp + ", label %" + ifName[1] +
                 ", label %" + ifName[2] + '\n' + ifName[1] + ":\n");
    cerr << "If parsed";
    linenum++;
    int tok = lex_tok(text.c_str());
    string cur_str = lex_str;
    Expr *exprPtr;
    while (tok != '}') {
        switch (tok) {
            case asg:
                exprPtr = new AsgExpr(cur_str, parseExpr(text.c_str()));
                gen.add_code(exprPtr->codeGen());
                delete exprPtr;
                break;
            case _print:
                exprPtr = parseArgs(text.c_str(), 1)[0];
                gen.add_code(exprPtr->codeGen());
                gen.add_code(generatePrint(exprPtr));
                delete exprPtr;
                break;
            case '\n':
                break;
            default:
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

    gen.add_code("br label %" + ifName[2] + '\n' + ifName[2] + ":\n");
}

int main(int argc, char *argv[]) {
    ios::sync_with_stdio(false);

    if (argc != 2) {
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

        // Reads through the token reaches the end of the file        
        while (tok != eof) {
            switch (tok) {
                case asg:
                    exprPtr = new AsgExpr(cur_str, parseExpr(text.c_str()));
                    cerr << "pos: " << pos << endl;
                    gen.add_code(exprPtr->codeGen());
                    delete exprPtr;
                    break;
                case _print:
                    exprPtr = parseArgs(text.c_str(), 1)[0];
                    gen.add_code(exprPtr->codeGen());
                    gen.add_code(generatePrint(exprPtr));
                    delete exprPtr;
                    break;
                case _while:
                    exprPtr = parseArgs(text.c_str(), 1)[0];
                    if (lex_tok(text.c_str()) != '{') {
                        throw InvalidExpr();  // Invalid Expression
                    }
                    if (lex_tok(text.c_str()) != '\n') {
                        throw InvalidExpr();  // Invalid Expression
                    }
                    cerr << "success";
                    runWhile(exprPtr, text, gen);
                    delete exprPtr;
                    break;

                case _if:
                    exprPtr = parseArgs(text.c_str(), 1)[0];
                    if (lex_tok(text.c_str()) != '{') {
                        throw InvalidExpr();  // Invalid Expression
                    }
                    if (lex_tok(text.c_str()) != '\n') {
                        throw InvalidExpr();  // Invalid Expression
                    }
                    cerr << "success";
                    runIf(exprPtr, text, gen);
                    delete exprPtr;
                    break;

                case '\n':
                    break;
                default:
                    throw InvalidExpr();  // Invalid Expression
            }
            tok = lex_tok(text.c_str());
            cur_str = lex_str;
            linenum++;
        }
        for (auto it : VarExpr::getVarList()) {
            gen.add_init(it);
        }
        io.writeFile(gen.get_code());

    } catch (const exception &e) {
        // Write the type of exception, then description.
        // Must be updated before submitting the assignment.
        /*
        exception_ptr p = make_exception_ptr(e);
        cerr << (p ? p.__cxa_exception_type()->name() : "null") << endl
             << e.what() << endl;
        cerr << "Line Number: " << linenum << endl;
        */
        cerr << "Line " << linenum << ": syntax error" << endl;
    }

    return 0;
}