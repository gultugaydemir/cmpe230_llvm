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

    } catch (const char* msg) {
        cerr << "Uncaptured error:\n" << msg << endl;
    }

    return 0;
}