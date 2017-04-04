/*
 * create swap-backed file, fork from it, and evict. 
 */

#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using namespace std;

int main() {
	// pointing to pinned memory
	cout << "HERE I AM" << endl;
	char * swap_backed = (char *) vm_map(nullptr, 1000);

	// make it dirty
	strcpy(swap_backed, "shakespeare.txt");

	char * spotHolder1 = (char *) vm_map(nullptr, 0);
	char * spotHolder2 = (char *) vm_map(nullptr, 3);

	// write to swapbacked files
	// to bring them into memory
	strcpy(spotHolder1, "data1.bin");

	// this second write should evict 
	// the dirty swap-backed file holding
	// shakespeare.txt
	cout << "EVICTED" << endl;
	strcpy(spotHolder2, "data2.bin");

	// bring swap-backed back
	cout << "README" << endl;
	for (int i = 0; i < 10; i++) {
		// print the first 10 characters of
		// shakespeare.txt
		cout << swap_backed[i] << endl;
	}// end for

	// At this point, we have a clean copy
	// of swap-backed in phy_mem and in file.

	// fork to itself
	// the state should be the same
	cout << "FORKING" << endl;
	fork();

	// evict spotHolder by reading the 2 spotholders
	cout << "EVICTED2" << endl;
	cout << spotHolder1[0] << endl;
	cout << "EVICTED3" << endl;
	cout << spotHolder2[0] << endl;

	return 0;
}