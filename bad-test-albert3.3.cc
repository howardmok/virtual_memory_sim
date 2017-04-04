// ONLY CATCHES N AND h. EXCLUDE THIS
/*
vm_map test and destroy. Also has some swapping stuff and clock (hopefully)!
*/

// Third test case written by Albert.


// Print the first line of text with a the last character
// modified to be a z
// Correct Output: Basicially just runs , no seg faults  

#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using namespace std;

int main() {
    const char * shakes = "shakespeare.txt";
    const char * data1 = "data1.bin";
    const char * data2 = "data2.bin";
    const char * data3 = "data3.bin";
    const char * data4 = "data4.bin";
    /* Allocate file-backed page from the arena */
    // filename and filename2 should point to different virtual pages
    char *p1 = (char *) vm_map(shakes, 512);   // referenced
    char * p2  = (char *) vm_map(data1, 1024);  // referenced
    char * p3  = (char *) vm_map(data2, 1024);  // referenced

    char * p4  = (char *) vm_map(data3, 512);  // bring this in, kick out p1

    cout << p1[0] << endl;// bring it back, kick out p2

    p2 = (char *) vm_map(data4,1024);   // make p2 = to something else
    cout << p2[0] << endl;

}