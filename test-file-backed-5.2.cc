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
    fork();

    vm_yield();

    char *filename = (char *) vm_map("shakespeare.txt", 0);

    char *filename2 = (char *) vm_map("submission.txt", 0);

    filename[0] = 'a'; //write, in cache

    exit(EXIT_SUCCESS);

    if (filename[0] == 'a') {
    	cout << "ggez?" << endl;
    }
}