#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"
#include <stdlib.h>

using namespace std;

int main()
{
    /* Allocate swap-backed page from the arena */
    char *filename = (char *) vm_map(nullptr, 0);

    char *filename2 = (char *) vm_map(nullptr, 0);

    char *filename3 = (char *) vm_map(nullptr, 0);

    char *filename4 = (char *) vm_map(nullptr, 0);

    /* Write the name of the file that will be mapped */
    strcpy(filename, "shakespeare.txt");

    strcpy(filename2, "shakespeare.txt");

    /* Map a page from the specified file */
    char *p = (char *) vm_map (filename, 0);

    char *r = (char *) vm_map (filename, 0);

    //File-backed 2
    char *q = (char *) vm_map("submission.txt", 5);

    pid_t child_pid = fork();
    cout << "child_pid: " << child_pid << endl;

    pid_t child_pid2 = fork();
    cout << "child_pid2: " << child_pid << endl;


    q[10] = 'g';

    //change first char
    filename[0] = 'a';

    filename2[0] = 'a';

    vm_yield();

    p[0] = 'a';

    q[0] = 'a';

    r[0] = 'b';

    filename2[2] = 'd';

    p[3] = 'r';

    vm_yield();

    p[1] = 'b';

    filename[0] = 'c';

    exit(EXIT_SUCCESS);

    if (p[1] == 'b') { //file backed should be changed?
        cout << "we did it" << endl;
    }
    if (filename[0] == 'c') { //swap backed shouldnt //NOTE : changed this to == from =
        cout << "why is this happening" << endl;
    }

    cout << "Still in a process" << endl;

    exit(EXIT_SUCCESS);

    cout << "Still alive" << endl;

    exit(EXIT_SUCCESS);

    cout << "Am I dead?!?" << endl; //should be dead

}