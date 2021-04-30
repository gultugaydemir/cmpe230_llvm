#include "../include/generator.h"
#include <iostream>
using namespace std;

Generator::Generator() {
    // Constructor for Generator.
}

string Generator::get_code() {
    // Safe return the result.
    return this->beg_code + this->choose_func + this->st_code +
           this->init_args + zero_args + this->gen_code + this->end_code;
}

string Generator::get_errcode() {
    // Safe return the result.
    return this->err_beg_code + this->st_code + this->gen_code + this->end_code;
}


void Generator::add_init(string text) {
    this->init_args += text + " = alloca i32\n";
    this->zero_args += "store i32 0, i32* " + text + '\n';
}

void Generator::add_code(string text) { this->gen_code += text; }