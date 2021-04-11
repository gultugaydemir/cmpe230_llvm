#ifndef GENERATOR_H
#define GENERATOR_H

#include <string>
using namespace std;

// Code generator class
class Generator {
   private:
    string code =
        "; ModuleID = 'mylang2ir'\n"
        "declare i32 @printf(i8*, ...)\n"
        "@print.str = constant [4 x i8] c\"%d\0A\00\"\n";

   public:
    Generator(const string& src);
    string get_code();
};

#endif