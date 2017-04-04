#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"
#include <stdlib.h>

using namespace std;

int main()
{
    char *filename = (char *) vm_map("shakespeare.txt", 0);

    char *filename2 = (char *) vm_map("shakespeare.txt", 0);

    char *filename4 = (char *) vm_map("submission.txt", 0);

    filename[0] = 'a';

    pid_t child_pid = fork(); //fork here

    filename2[0] = 'b';

    vm_yield(); //switch here

    if (filename[0] == 'a') { //should be true, file is shared
        cout << "Fork" << endl;
    }

    if (filename2[0] = 'b') { //should also be true, file is shared even after fork for file
        cout << "spork" << endl;
    }

    filename4[0] = 'c'; //should evict

    exit(EXIT_SUCCESS);

    if (filename4[0] = 'c') { //should be true since files shared across processes, even when exited
        cout << "yasssss" << endl;
    }

    filename2[0] = 'd'; //should evict too
}