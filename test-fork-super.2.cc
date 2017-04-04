#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using namespace std;
/*
 * Simple fork test 
 */

int main()
{
    /* Allocate swap-backed page from the arena */
    cout << "creating my first swap backed file!!!!!!" << endl;
    char *filename = (char *) vm_map(nullptr, 0);

    /* Write the name of the file that will be mapped */
    cout << "dirying my first swap backed child!!!!!!" << endl;
    strcpy(filename, "shakespeare.txt");

    /* Map a page from the specified file */
    cout << "creating my first filebacked child!!!!!!!" << endl;
    char *p = (char *) vm_map (filename, 0);

    cout << "reading my first filebacked child!!!!!!!!" << endl;
    cout << "should evict..." << endl;
    cout << p[0] << endl;   

    cout << "FORKING !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
    pid_t child_pid = fork();

    
    cout << "child pid:" << child_pid << endl;

    if (child_pid == 0) {
        cout << "Child thread!" << endl;
        cout << "should not be capital S: " << filename[0] << endl;
        vm_yield();
    }
    if (child_pid != 0) {
        cout << "Parent thread!" << endl;

        cout << "evict the current thing and bring the dirty swapfile back!!!!!!!!!!!!!!!!!!!!!!" << endl;
        cout << filename[0] << endl;
        filename[0] = 'S';
        // bring 
        vm_yield();
    }

}