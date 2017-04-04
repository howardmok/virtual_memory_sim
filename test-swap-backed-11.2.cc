/*
 * TESTCASES 9 - 13 SHOULD BE THE SAME STATE
 * create swap-backed file, and write to it. 
 * get it clock swept to update its reference bit to 1. 
 * then preform a read
 */

#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using namespace std;

int main() {
	// pointing to pinned memory
	char * placeHolder = (char *) vm_map(nullptr, 0);
	
	// make it dirty
	strcpy(placeHolder, "data1.bin");
	placeHolder[0] = 'A';


	// pointing to pinned memory
	char * swap_backed = (char *) vm_map(nullptr, 1000);

	// make it dirty
	strcpy(swap_backed, "shakespeare.txt");
	swap_backed[0] = 'A';

	// bring something else in so swap-backed gets clocked once
	// but not evicted
	char * placeHolder2 = (char *) vm_map(nullptr, 0);
	strcpy(placeHolder2, "data2.bin");
	placeHolder2[0] = 'B';

	cout << "Should be an A: " << swap_backed[0] << endl;


	return 0;
}