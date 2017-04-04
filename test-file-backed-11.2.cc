// test case 10 and 11 are similar

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
	file_backed2[0] = 'B';

	// filebacked1 and filebacked2 should both be a reference bit 0
	file_bro[0] = 'A';
	cout << "should be an A: " << file_backed1[0] << endl;
	cout << "should def be an A: " << file_bro[0] << endl;
	// file backed1 and filebacked2 should be reference bit 1


	cout << swap_backed[0] << endl;	// this should evict file_backed1 and file_backed2 

	// file_backed2 is clock swept
	// write
	file_backed2[0] = 'C';	
}