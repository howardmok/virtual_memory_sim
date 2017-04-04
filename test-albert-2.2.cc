#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

/*
This test case will try to test the functionality of vm_pager.cc
where we have dirty swap-backed and file-backed data.
*/

using namespace std;

// NOTE : we changed vm_map

int main() {
    /* Allocate swap-backed page from the arena */
    cout << "creating my first swap-backed file! -----------" << endl;
    char *filename = (char *) vm_map(nullptr, 0);

    /* Write the name of the file that will be mapped */
    // filename is dirty
    cout << "dirtying it uppppppppppppppppppppp! -----------" << endl;
    strcpy(filename, "data2.bin");

    // create another swap-backed page pointing to pinned memory
    cout << "forking itttt ---------------------------------" << endl;
    fork();

 	// point filename2 to pinned 0 memory 
    cout << "creating my second swap-backed file! ----------" << endl;
    char *filename2 = (char *) vm_map(nullptr, 0);

    // p and p2 point to the same thing
    cout << "creating my first file-backed file! -----------" << endl;
    char *p = (char *) vm_map (filename, 1);

    cout << "creating my second file-backed backed file! -----------" << endl;
    char *p2 = (char *) vm_map (filename2, 1);

    cout << "dirtying up my first file backed file! ---------" << endl; 
    p[0] = 'Q';
    // p2 should've gotten evicted

    cout << "reading my second filebacked file! -------------" << endl;
    cout << p2[0] << endl;
    // p2 is invalid. vm destroy called

    // filename now has data for data1 instead of shakespeare
    cout << "overwriting my first swap-backed file with new data! ---" << endl;
    strcpy(filename, "data1.bin");

    cout << "should be an 8, not S: " << filename[0] << endl;
    // p2 should get evicted

    cout << "should be a capital Q: " << p[0] << endl;
    cout << p2[0] << endl;
    // filename with data1 should NOT be written back to swap file undirty
   
   	filename[0] = 'H';

   	cout << "should be a capital Q: " << p[0] << endl;
    cout << "should be a S: " << p2[0] << endl;
    // filename with data1 should be written back to swap file undirty

    cout << "should be a capital H: " << filename[0] << endl;
}