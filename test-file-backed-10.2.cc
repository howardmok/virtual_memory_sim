// test case 10 and 11 are similar

#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using namespace std;

int main() {
	char * swap_backed = (char *) vm_map(nullptr, 1000);	// swapbacked file, pointing to pinned memory
	strcpy(swap_backed, "data1.bin");						// swapbacked file is dirty. no longer pointing to pinned. 

	char * file_backed1 = (char *) vm_map(swap_backed, 0);	// create file dependency. should be in file, not in phys_mem yet

	char * file_bro = (char *) vm_map(swap_backed, 0);		// nothing should get evicted here. same file dependency as file_backed1

	char * file_backed2 = (char *) vm_map(file_bro, 0);		// file_bro is in a valid part of the arena. It contains a lot of stuff from data1.bin
															// file_bro, however, is not legal
							
	file_backed2[0] = 'B';									// fail! file_backed2 is not valid. 

	file_bro[0] = 'A';										
	cout << "should be an A: " << file_backed1[0] << endl;	
	cout << "should def be an A: " << file_bro[0] << endl;	

	cout << swap_backed[0] << endl;	
	cout << "should be an A: " << file_backed2[0] << endl;	
} // end main()