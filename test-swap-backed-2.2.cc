/*
 * create swap-backed file, read from it, and then fork from it
 */

#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using namespace std;

int main() {
	// pointing to pinned memory
	cout << "first vm_map" << endl;
	char * swap_backed = (char *) vm_map(nullptr, 1000);

	// make it dirty
	cout << "dirtying it up" << endl;
	strcpy(swap_backed, "shakespeare.txt");

	cout << "second vm_map" << endl;
	char * spotHolder1 = (char *) vm_map(nullptr, 0);

	cout << "third vm_map" << endl;
	char * spotHolder2 = (char *) vm_map(nullptr, 3);

	// write to swapbacked files
	// to bring them into memory
	cout << "dirtying it up2" << endl;
	strcpy(spotHolder1, "data1.bin");

	// this second write should evict 
	// the dirty swap-backed file holding
	// shakespeare.txt
	cout << "dirtying it up3" << endl;
	strcpy(spotHolder2, "data2.bin");

	// bring swap-backed back
	for (int i = 0; i < 10; i++) {
		// print the first 10 characters of
		// shakespeare.txt
		cout << swap_backed[i] << endl;
	}// end for

	// At this point, we have a clean copy
	// of swap-backed in phy_mem and in file.

	// fork to itself
	// the state should be the same
	fork();

	return 0;
}