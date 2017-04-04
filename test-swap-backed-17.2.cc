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
    char *filename2 = (char *) vm_map(nullptr, 0);
    char *filename3 = (char *) vm_map(nullptr, 0);
    char *filename4 = (char *) vm_map(nullptr, 0);

    for (unsigned int i = 0; i < 255; i++) {
        fork();
    }

    char *filename5 = (char *) vm_map(nullptr, 0); //eager swap reservation should return -1 cause no space

}