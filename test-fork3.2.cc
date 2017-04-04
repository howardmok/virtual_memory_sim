#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"
#include <stdlib.h>

using namespace std;
/*
 * Simple fork test 2
 */

int main()
{
    /* Allocate swap-backed page from the arena */
    char *filename = (char *) vm_map(nullptr, 0);

    /* Write the name of the file that will be mapped */
    strcpy(filename, "shakespeare.txt");

    /* Map a page from the specified file */
    char *p = (char *) vm_map (filename, 0);

    pid_t child_pid = fork(); //fork

    if (child_pid == 0) { //true always
        cout << "Child thread!" << endl;
        exit(EXIT_SUCCESS); //kill parent
    }
    if (child_pid != 0) { //should never come here
        cout << "My own child terminated me. I should not be here... :^(" << endl;
    }

    cout << "Not according to plan..." << endl; //should never reach here because child keeps respawning

}