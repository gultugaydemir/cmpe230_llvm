#include "../include/fileio.h"

#include <fstream>
using namespace std;

// Taking the files with .my extension as the input, creating the .ll file output.
FileIO::FileIO(const char* rawName) {
    inFile = string(rawName);
    try{
        if (this->inFile.substr(this->inFile.find_last_of('.')) != ".my")
            throw InvalidExt();
    } catch (const exception &e) {
        // Throw error for find_last_of
        throw InvalidExt();
    }
    this->outFile =
        this->inFile.substr(0, this->inFile.find_last_of('.')) +
        ".ll";
}

// Fast file reading function
string FileIO::readFile() {
    std::ifstream in;
    in.exceptions(ifstream::failbit);
    in.open(this->inFile, std::ios::in | std::ios::binary);
    std::string contents;
    in.seekg(0, std::ios::end);
    contents.resize(in.tellg());
    in.seekg(0, std::ios::beg);
    in.read(&contents[0], contents.size());
    in.close();
    return contents;
}

// Text file writing function
void FileIO::writeFile(const string& text) {
    ofstream out;
    out.exceptions(ofstream::failbit);
    out.open(this->outFile);
    out << text;
    out.close();
}