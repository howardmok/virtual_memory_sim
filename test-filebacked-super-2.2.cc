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
	strcpy(filename, "data4.bin");

	char *filename2 = (char *) vm_map(nullptr, 2);
	strcpy(filename2, "data4.bin");

	char *filename3 = (char *) vm_map(nullptr, 17);
	strcpy(filename3, "data4.bin");

	// all different?
	char *filebacked = (char *) vm_map(filename, 0);
	char *filebro = (char *) vm_map(filename2, 0);
	char *filesis = (char *) vm_map(filename3, 0);

}