/*
 * TESTS 6 - 8 ARE VERY SIMILAR. TESTING SAME CONCEPTS
 * create file-backed file, get it to clock sweep once
 * from it and read from it. then get the file evicted.
 */

#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using namespace std;

int main() {
	// pointing to pinned memory
	char * swap_backed = (char *) vm_map(nullptr, 1000);
	strcpy(swap_backed, "shakespeare.txt");					// dirty

	char * file_backed1 = (char *) vm_map(swap_backed, 5);	// occupy the second physical memory page

	char * file_bro = (char *) vm_map(swap_backed, 5);		// nothing should get evicted here, right?

	char * file_backed2 = (char *) vm_map(file_bro, 7);		// ok, swap_backed should get evicted here and written
															// back to swapfile

	// filebacked1 and filebacked2 should both be a reference bit 0
	cout << file_bro[0] << endl;
	// file backed1 and filebacked2 should be reference bit 1


	cout << swap_backed[0] << endl;	// this should evict file_backed1 and file_backed2 

}