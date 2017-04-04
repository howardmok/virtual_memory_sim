/*
vm_map test 
file-backed pages with the same filename and blocksize 
should share the same page
*/

// First test case written by Albert.
// Not too sure what these numbers mean,
// but just trying to see if it runs on the AG. 


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

// NOTE : we changed vm_map block from 1 to 0
int main() {
	char *filename = "data1.bin";
    /* Allocate file-backed page from the arena */
    // filename and filename2 should point to the same virtual page
    char * p1 = (char *) vm_map(filename, 1);
    char * p2 = (char *) vm_map(filename,1);

    // filename3 should point to its own virtual page
    char * p3 = (char *) vm_map(filename,0);

    if (p1 == nullptr && p2 == nullptr && p3 == nullptr) {
        cout << "All nullptr... as expected!" << endl;
        return 0;
    }
    cout << "Huh? Something went wrong." << endl;
    cout << "This shouldn't run...?" << endl;

    cout << "p1: " << p1[0] << endl;
    cout << "p2: " << p2[0] << endl;
    cout << "p3: " << p3[0] << endl;

    /* Print the first speech from the file */
    for (unsigned int i=0; i<23; i++) {
		cout << p1[i];
	}
	// I think this should modify the last character
	// of the first line. 
	p1[23] = 'z';
	cout << p2[23] << endl;
    
    cout << endl;
    cout << endl;
    
    for(unsigned int i = 0; i < 24; i++) {
		cout << p3[i];
	}
	cout << endl;
    return 0;
}
