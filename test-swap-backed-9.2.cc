/*
 * TESTCASES 9 - 13 SHOULD BE THE SAME STATE
 * create swap-backed file, and write to it
 * then read it
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

	for(int i = 0; i < 20; i++) {
		swap_backed[i] = 'H';
	}

	for(int i = 0; i < 20; i++) {
		cout << swap_backed[i] << " ";
	}
	cout << endl;    
	
	return 0;
}