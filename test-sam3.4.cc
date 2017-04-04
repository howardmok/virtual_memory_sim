/*
 * TESTS whether or not things are saved back after destroy
 *
 */

#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using namespace std;

int main() {
    char * swap_backed = (char *) vm_map(nullptr, 2000);    // points to pinned memory
    
    strcpy(swap_backed, "shakespeare.txt");                 // swap_backed gets moved to second page of memory

    if(fork()) {
        char * file_backed1 = (char *) vm_map(swap_backed, 0);
        strcpy(file_backed1, "data1.bin");
        exit(EXIT_SUCCESS);
    } else {
        char * file_backed1 = (char *) vm_map(swap_backed, 0);
        char * file_backed2 = (char *) vm_map(file_backed1, 2); // points to pinned memory
        string s1, s2;
        s1 = s2 = "";
        for(int i = 0; i < 25; ++i) {
            s1 += file_backed1[i];
            s2 += file_backed2[i];
        }
        cout << s1 << endl;
        cout << s2 << endl;
        exit(EXIT_SUCCESS);
    }
}