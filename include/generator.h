#ifndef GENERATOR_H
#define GENERATOR_H

#include <string>
using namespace std;

// Code generator class
class Generator {
   private:
    const string beg_code =
        "; ModuleID = 'mylang2ir'\n"
        "declare i32 @printf(i8*, ...)\n"
        "@print.str = constant [4 x i8] c\"%d\\0A\\00\"\n\n"
        "define i32 @main() {\n";

    string init_args = "";
    string zero_args = "";

    string gen_code = "";

    const string end_code = "\nret i32 0\n}";

   public:
    Generator();
    string get_code();
    void add_init(string args);
    void add_code(string text);
};

#endif