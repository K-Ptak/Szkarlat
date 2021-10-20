#include <iostream>
#include <vector>
#include "include/color.hpp"
using namespace std;

int main()
{
    int a;
    for (;;) {
        cout << dye::red("Hello World!") << endl;
        cout << dye::light_red("Hello World!") << endl;
        cout << dye::yellow("Hello World!") << endl;
        cout << dye::green("Hello World!") << endl;
        cout << dye::blue("Hello World!") << endl;
        cout << dye::aqua("Hello World!") << endl;
        cout << dye::purple("Hello World!") << endl;
        
    }
}