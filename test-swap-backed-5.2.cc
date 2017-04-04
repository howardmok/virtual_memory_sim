/*
 * TESTS 5 - 8 should all have the same state in the FSM. 
 * create swap-backed file, get it to clock sweep once
 * from it and write to reset it
 */

#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using namespace std;

int main() {
	// pointing to pinned memory
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
	strcpy(spotHolder2, "data2.bin");

	// at this point, swap-backed should be evicted and 
	// the reference_bit of spotHolder1 should be 0, 
	// and the reference_bit of spotHolder2 should be 1

	// write from spotHolder1 to update its reference_bit
	// to 1. 
	spotHolder1[0] = 'A';

	return 0;
}