#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"
#include <stdlib.h>

using namespace std;

int main() {
    // create a swap-backed file
    int i = 0;
    while(i < 200) {
        char *words = (char *) vm_map(nullptr, 0);
        words[0] = 'i';
        if(fork()) {
            if(fork()) {
                if(fork()) {
                    vm_yield();
                } else {
                    vm_yield();
                }
            } else {
                if(fork()) {
                    vm_yield();
                } else {
                    vm_yield();
                }
            }
            cout << "parent is running" << endl;
            char *farewell = (char *) vm_map(nullptr, 0);
            strcpy(farewell, "passing off the torch");
            cout << farewell << endl;
            vm_yield();
            cout << "nothing should have ran before this" << endl;
            exit(EXIT_SUCCESS);
        } else {
            if(fork()) {
                if(fork()) {
                    vm_yield();
                } else {
                    vm_yield();
                }
            } else {
                if(fork()) {
                    vm_yield();
                } else {
                    vm_yield();
                }
            }
            char *hello = (char *) vm_map(nullptr, 0);
            strcpy(hello, "this should never happen");
            cout << hello << endl;
            exit(EXIT_SUCCESS);
        }
    }
}