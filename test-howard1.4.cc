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

    /* Map a page from the specified file */
    char *p = (char *) vm_map (filename, 0);

    //Swap-backed page mapped to block 1
    char *filename2 = (char *) vm_map(nullptr, 1);

    strcpy(filename2, "shakespeare.txt");

    //File-backed mapped to block 1
    char *q = (char *) vm_map(filename2, 1);

    //change first char
    filename[0] = 'a';

    filename[1] = 'b';

    p[0] = 'a';

    filename2[0] = 'a';

    q[0] = 'a';

    //check if they are the same and if you can access
    if (filename[0] == p[0]) {
        cout << "Same block, different virtual page types." << endl;
    }

    //check if they are the same and if you can access
    if (filename[0] == filename2[0]) {
        cout << "Blocks shouldn't matter cause swap-backed." << endl;
    }

    //check if they are the same and if you can access
    if (p[0] == q[0]) {
        cout << "2 file-backed in different blocks." << endl;
    }

    //check if they are the same and if you can access
    if (filename[0] == q[0]) {
        cout << "Different block and different virtual page type." << endl;
    } 

    //check if they are the same and if writing to swap-backed does anything, shouldn't
    if (filename[1] == p[1]) {
        cout << "Swap-backed changes original file" << endl;
    }   

    p[1] = 'c';

    //check if file-backed change changes orig file and affects swap-backed page, shouldn't
    if (filename[1] == 'c') {
        cout << "Swap-backed is changed by file-backed in same block" << endl;
    }

    //check if file-backed change changes orig file and affects swap-backed page, shouldn't
    if (filename2[1] == 'c') {
        cout << "I guess this one too" << endl;
    }

    //check if file-backed change changes orig file and affects file-backed page, shouldn't (?)
    if (q[1] == 'c') {
        cout << "File-backed is changed by file-backed in different block" << endl;
    }
}