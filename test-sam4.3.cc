/*                                   *
 * tests whether or not things point *
 * to the same page after eviction   *
 *                                    */
#include "vm_app.h"

#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using namespace std;

int main()
{
    fork();
    /* Allocate swap-backed page from the arena */
    /* 1st page in the cache */
    char *filename = (char *) vm_map(nullptr, 0);


    /* Write the name of the file that will be mapped */
    strcpy(filename, "shakespeare.txt");

    /* Map a page from the specified file */
    /* 2nd page in the cache */
    char *shakespeare = (char *) vm_map (filename, 0);
    
    shakespeare[0] = 'a';

    char *filename1 = (char *) vm_map(nullptr, 0);

    strcpy(filename1, "what");

    cout << filename[0] << endl;
    filename[0] = 'b';
    cout << filename[0] << endl;
    exit(EXIT_SUCCESS);
}

