/*
 * tests a things with fork and whether or not things work
 */

#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"
#include <stdlib.h>

using namespace std;

int main() {
    char * swap_backed = (char *) vm_map(nullptr, 1000);    // points to pinned memory
    
    strcpy(swap_backed, "shakespeare.txt");                 // swap_backed gets moved to second page of memory

    char * file_backed = (char *) vm_map(swap_backed, 0);

    if(fork()) {
        vm_yield();
        file_backed[0] = '!';
        for(int i = 0; i < 26; ++i) {
            cout << file_backed[i];
        }
        cout << "ending parent" << endl;
        exit(EXIT_SUCCESS);
    } else {
        char * swap_backed2 = (char *) vm_map(nullptr, 1000); // points to pinned memory
        strcpy(swap_backed2, "where my doge at?");            // swap_backed gets moved to the third page of memory
        for(int i = 0; i < 26; ++i) {
            cout << file_backed[i];
        }
        file_backed[0] = file_backed[2] = file_backed[4] = '?';
        cout << "child ran" << endl;
        vm_yield();
        cout << "child ending1\n";
        vm_yield();
        exit(EXIT_SUCCESS);
    }
}