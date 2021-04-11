#ifndef FILEIO_H
#define FILEIO_H

#include <string>
using namespace std;

// File handling for the compiler
class FileIO {
   private:
    string inFile;
    string outFile;

   public:
    FileIO(const char* rawName);
    string readFile();
    void writeFile(const string& text);
};

#endif