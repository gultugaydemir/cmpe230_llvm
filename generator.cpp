#include "generator.h"

#include <iostream>
using namespace std;

Generator::Generator(const string& src) {
    // Constructor for Generator.
    cout << src << endl;
}

string Generator::get_code() {
    // Safe return the result.
    return this->code;
}