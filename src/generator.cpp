#include "../include/generator.h"
#include <iostream>
using namespace std;

// Constructor for Generator.
Generator::Generator() {
}

// Concatenate the code blocks and return the result.
string Generator::get_code() {
    return this->beg_code + this->choose_func + this->st_code +
           this->init_args + zero_args + this->gen_code + this->end_code;
}

// Returns the IR code that prints the syntax error.
string Generator::get_errcode() {
    return this->err_beg_code + this->st_code + this->gen_code + this->end_code;
}

// Initialization of the variables.
void Generator::add_init(string text) {
    this->init_args += text + " = alloca i32\n";
    this->zero_args += "store i32 0, i32* " + text + '\n';
}

// Adding the codes throughout the parsing.
void Generator::add_code(string text) { this->gen_code += text; }