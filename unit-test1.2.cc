/*
 * make a bunch of swap backed
 */

#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using namespace std;

int main() {
	char * swap_backed = (char *) vm_map(nullptr, 1000);
	char * swap_backed1 = (char *) vm_map(nullptr, 1000);
	char * swap_backed2 = (char *) vm_map(nullptr, 1000);

	strcpy(swap_backed1, "shakespeare.txt");

	char * file_backed = (char *) vm_map(swap_backed1, 0);
	cout << file_backed[0] << endl;
	file_backed[0] = 'A';
}