#include <sipster/sip.h>
#include <string>
#include <iostream>

using namespace std;

int main(int argc, char ** argv) {

    string s;
    if(s.empty()) {
        cout << "EMPTY" << endl;
    } else {
        cout << "NOT EMPTY" << endl;
    }

    s = "";
    if(s.empty()) {
        cout << "EMPTY" << endl;
    } else {
        cout << "NOT EMPTY" << endl;
    }

    return 0;
}
