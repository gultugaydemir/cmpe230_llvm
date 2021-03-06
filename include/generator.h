#ifndef GENERATOR_H
#define GENERATOR_H

#include <string>
using namespace std;

// Code generator class
class Generator {
   private:
   
   // Module name and prototype of print statement.
    const string beg_code =
        "; ModuleID = 'mylang2ir'\n"
        "declare i32 @printf(i8*, ...)\n"
        "@print.str = constant [4 x i8] c\"%d\\0A\\00\"\n\n";

   // Module name and prototype of print statement for error messages.
    const string err_beg_code =
        "; ModuleID = 'mylang2ir'\n"
        "declare i32 @printf(i8*, ...)\n"
        "@print.str = constant [23 x i8] c\"Line %d: syntax error\\0A\\00\"\n\n";

   // Prototype for choose function.
    const string choose_func =
        "define i32 @choose(i32 %e1, i32 %e2, i32 %e3, i32 %e4){\n"
        "%dt1 = icmp slt i32 0, %e1\n"
        "br i1 %dt1, label %ret_e3, label %cmp\n"
        "cmp:\n"
        "%dt2=icmp eq i32 0, %e1\n"
        "br i1 %dt2, label %ret_e2, label %ret_e4\n"
        "ret_e2:\n"
        "ret i32 %e2\n"
        "ret_e3:\n"
        "ret i32 %e3\n"
        "ret_e4:\n"
        "ret i32 %e4\n"
        "}\n\n";

    // Structure for main function
    const string st_code = "define i32 @main() {\n";

    string init_args = "";
    string zero_args = "";

    string gen_code = "";

    // Structure for ret instruction
    const string end_code = "\nret i32 0\n}";

   public:
    Generator();
    string get_code();
    string get_errcode();
    void add_init(string args);
    void add_code(string text);
};

#endif
