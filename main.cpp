#include <iostream>
using namespace std;

auto lambda = [](auto x, auto y) {return x + y;};

int main(int argc, char *argv[]){
    cout << "Hello, World!\n";
    cout << lambda(5, 7) << endl;
    return 0;
}