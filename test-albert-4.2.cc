#include <iostream>
#include <cstring>
#include <unistd.h>
#include <cstring>
#include "vm_app.h"

using namespace std;
// NOTE : we changed vm_map block from 1 to 0
int main() {
	// second value should be ignored b/c swap-backed file
	char * filename = (char *) vm_map(nullptr, 500);

	strcpy(filename, "shakespeare.txt");

	char * fileBackedParent = (char *) vm_map(filename, 0);

	pid_t child = fork();
	pid_t child2 = fork();
	pid_t child3 = fork();

	cout << "pid_t of child 1: " << child << endl;
	cout << "pid_t of child 2: " << child2 << endl;
	cout << "pid_t of child 3: " << child3 << endl;
	// at this point, dependencies should be pointing to
	// the same block

	// now, parent should point to something else are cleared
	fileBackedParent[0] = 'H';

	vm_yield();
	// should be on a different process

	// child is reading
	cout << fileBackedParent[0];

	// child has it's own block 
	fileBackedParent[0] = 'I';
	cout << endl;
	vm_yield();
	

	while( fileBackedParent[0] == 'H' || fileBackedParent[0] == 'I' ) {
		// keep yielding until we get something different
		vm_yield();
	}

	// At child2 now
	// child2 is reading
	cout << fileBackedParent[0];

	// increase the size of the arena
	vm_map(fileBackedParent, 1);
	
	// switch to something else
	fork();

	cout << fileBackedParent[0] << endl;
}