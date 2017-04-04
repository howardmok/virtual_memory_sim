/*
vm_map test and destroy
*/

// Second test case written by Albert.
// Catches bugs: N, h


// Print the first line of text with a the last character
// modified to be a z
// Correct Output:
//		St. Crispen's Day Speecz
//
//
//		St. Crispen's Day Speech

#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using namespace std;

int main()
{
	char *filename = "shakespeare.txt";
    /* Allocate file-backed page from the arena */
    // filename and filename2 should point to different virtual pages
    if (true) {
        char *p = (char *) vm_map(filename, 1024);
        p[23] = 'z';
        for(uint i = 0; i < 24; i++) {
            cout << p[i] << endl;
        }
    } // unsure if the application dies here or not. I think it should deallocate the memory here??

    char *p2 = (char *) vm_map(filename,1024);
    
    for(uint i = 0; i < 24; i++) {
		cout << p2[i] << endl;
	}
}