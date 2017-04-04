#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using namespace std;

int main()
{
    /* Allocate swap-backed page from the arena */
    char *filename = (char *) vm_map(nullptr, 0);

    /* Write the name of the file that will be mapped */
    strcpy(filename, "shakespeare.txt");

    for (int i = 0; i < 4; i++) {
        pid_t child_pid = fork();
        cout << "Forked bruh" << endl;
        filename[10] = '!';
        vm_yield();
        cout << "I am child" << i << endl;
        filename[9] = '?';
    }

}