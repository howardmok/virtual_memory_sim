#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using namespace std;
/*
 * Simple fork test 
 */

int main()
{
    /* Allocate swap-backed page from the arena */
    cout << "creating my first swap backed file!!!!!!" << endl;
    char *filename = (char *) vm_map(nullptr, 0);

    /* Write the name of the file that will be mapped */
    cout << "dirying my first swap backed child!!!!!!" << endl;
    strcpy(filename, "shakespeare.txt");
    
    if (fork()) {
        cout << "parent" << endl;
        strcpy(filename, "data1.bin");
        for (unsigned int i = 0; i < 20; i++) {
            cout << filename[i] << " ";
        }
        cout << endl;
    } else {
        cout << "child" << endl;
        for (unsigned int i = 0; i < 20; i++) {
            cout << filename[i] << " ";
        }
        cout << endl;
    }

}