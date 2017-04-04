/*
 * TESTS 6 - 8 ARE VERY SIMILAR. TESTING SAME CONCEPTS
 * create file backed file, get it to clock sweep once
 * from it and read from it. then get the file evicted.
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

    if(fork()) {
        vm_yield();
    }

    cout << swap_backed[0] << endl;
}