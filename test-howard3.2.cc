#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using namespace std;

int main()
{
    /* Allocate swap-backed page from the arena */
    char *filename = (char *) vm_map(nullptr, 0);

    char *filename2 = (char *) vm_map(nullptr, 0);

    char *filename3 = (char *) vm_map(nullptr, 0);

    /* Write the name of the file that will be mapped */
    strcpy(filename, "shakespeare.txt");

    strcpy(filename2, "shakespeare.txt");

    strcpy(filename3, "submission.txt");

    /* Map a page from the specified file */
    char *p = (char *) vm_map (filename, 0);

    char *r = (char *) vm_map (filename, 1);

    //File-backed 2
    char *q = (char *) vm_map("submission.txt", 5);

    filename[0] = 'a';

    filename2[0] = 'b'; //evict filename

    filename3[1] = 'c'; //evict filename2

    p[0] = 'r'; //evict filename3

    r[1] = 'q'; //no evict

    q[3] = 'p'; //evict p
}