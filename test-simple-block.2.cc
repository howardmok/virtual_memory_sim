#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using namespace std;

int main()
{
    /* Allocate swap-backed page from the arena */
    char *filename = (char *) vm_map(nullptr, 0);

    strcpy(filename, "shakespeare.txt");

    char *r = (char *) vm_map (filename, 1); 

    filename[0] = 'a';

    r[1] = 'b';

}