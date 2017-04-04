/*
 * TEST CASE 15 and 16 test the same state
 * create swap-backed file, and write to it. 
 * get it evicted and write to it 
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
	swap_backed[0] = 'A';


	// pointing to pinned memory
	char * placeHolder = (char *) vm_map(nullptr, 0);
	// make it dirty
	strcpy(placeHolder, "data1.bin");
	placeHolder[0] = 'A';


	// bring something else in so swap-backed evicted
	char * placeHolder2 = (char *) vm_map(nullptr, 0);
	strcpy(placeHolder2, "data2.bin");
	placeHolder2[0] = 'B';

	swap_backed[1] = 'B';

	return 0;
}