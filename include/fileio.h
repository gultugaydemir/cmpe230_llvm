#ifndef FILEIO_H
#define FILEIO_H

#include <string>
using namespace std;

// Handles invalid extensions for the input
struct InvalidExt : public exception {
    const char* what() const throw() {
        return "Invalid File Extension";
    }
};

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