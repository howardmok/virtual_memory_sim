/*
 * TESTS that a thing can read a filename from two different blocks
 */

#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using namespace std;

int main() {

    // TEST DESTROY
    char * swap_backed1 = (char *) vm_map(nullptr, 1000);    // points to pinned memory
    char * swap_backed2 = (char *) vm_map(nullptr, 2000);
    swap_backed1[4090] = 's';
    swap_backed1[4091] = 'h';
    swap_backed1[4092] = 'a';
    swap_backed1[4093] = 'k';
    swap_backed1[4094] = 'e';
    swap_backed1[4095] = 's';
    if(fork()) {
        strcpy(swap_backed2, "peare.txt");
        char * filename = (char *) vm_map(swap_backed1 + (4090), 0);
        for(int i = 0; i < 100; ++i) {
            cout << filename[i];
        }
        cout << endl;
        exit(EXIT_SUCCESS);
    } else {
        strcpy(swap_backed2, "");
        char * filename = (char *) vm_map(swap_backed1 + (4090), 0);
        for(int i = 0; i < 100; ++i) {
            cout << filename[i];
        }
        cout << endl;
        exit(EXIT_SUCCESS);
    }
}