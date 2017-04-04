/*
 * create swap-backed file and clock evict it with no writes 
 * and bring it back with reads
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
	
	cout << swap_backed[0] << endl;
	cout << spotHolder1[0] << endl;
	cout << spotHolder2[0] << endl;

	return 0;
}