#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"
#include <stdlib.h>

using namespace std;
/*
 * Simple fork test 2
 */

int main() {
	char *filename = (char *) vm_map(nullptr, 0);
	strcpy(filename, "haha.txt");	// invalid
	cout << "invalid: " << filename[0] << endl; 
	strcpy(filename, "data4.bin");

	char *fb1 = nullptr;

	// first filebacked file. should remain in file
	cout << "forloop" << endl;
	for (unsigned int i = 0; i < 3; i++) {
		fb1 = (char *) vm_map(filename, 0); 
	}
	cout << "end forloop" << endl;
	char *fb2 = (char *) vm_map(filename, 1);
	char *fb3 = (char *) vm_map(filename, 0);	// should be pointing to same thing as fb1

	cout << "fb3: " << fb3[0] << endl;
	cout << "fb2: " << fb2[0] << endl;
	cout << "fb1 (should be a K): " << fb1[0] << endl;
	
	fb1 = (char *) vm_map(filename, 0); // should be resident
	cout << "fb1 (should be a K): " << fb1[0] << endl;
}