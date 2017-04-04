#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"
#include <stdlib.h>

using namespace std;

int main() {
    // create a swap-backed file
    char *words = (char *) vm_map(nullptr, 0);
    
    strcpy(words, "hello, albert was here");

    if(fork()) {
        char* words2 = (char *) vm_map(nullptr, 0);

        words2[0] = 'g';

        cout << words[8] << endl;
        
        vm_yield();

        cout << words2[2] << endl;

        cout << words << endl;

        exit(EXIT_SUCCESS);
    } else {
        cout << words << endl;

        strcpy(words, "hello, now it's sam!");

        vm_yield();

        cout << words << endl;

        char * moreWords = (char *) vm_map(nullptr, 0);
        moreWords[1] = 'q';
        cout << moreWords[1] << endl;
        exit(EXIT_SUCCESS);
    }
}