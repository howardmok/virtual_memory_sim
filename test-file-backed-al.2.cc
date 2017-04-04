#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"
#include <stdlib.h>

using namespace std;

int main() {
	// create a swap-backed file
    char *filename = (char *) vm_map(nullptr, 0);
    strcpy(filename, "data1.bin");

    if (fork()) {
    	cout << "parent process" << endl;
    	char *filebacked = (char *) vm_map(filename, 0);
    	filebacked[0] = 'S';	// bring the filebacked file into memory and dirty it
    	exit(EXIT_SUCCESS);
    }
    else {
    	cout << "child process" << endl;
    	// filebacked rw should still be in memory.
    }
}