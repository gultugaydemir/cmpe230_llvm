#include <iostream>

#include "../include/fileio.h"
#include "../include/generator.h"
using namespace std;

int main(int argc, char* argv[]) {
    ios::sync_with_stdio(false);

    if (argc < 2) {
        cerr << "Please enter a filename!\n";
        return 0;
    }

    try {
        FileIO io(argv[1]);

        Generator gen = Generator(io.readFile());
        io.writeFile(gen.get_code());

    } catch (const exception& e) {
        // Write the type of exception, then description.
        // Must be updated before submitting the assignment.
        exception_ptr p = make_exception_ptr(e);
        cerr << (p ? p.__cxa_exception_type()->name() : "null") << endl
             << e.what() << endl;
    }

    return 0;
}