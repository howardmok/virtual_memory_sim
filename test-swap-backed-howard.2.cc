#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"
#include <stdlib.h>

using namespace std;

int main() {
	// pointing to pinned memory
	char * swap_backed = (char *) vm_map(nullptr, 1000);

	// make it dirty
	strcpy(swap_backed, "shakespeare.txt");

	char * swap_backed2 = (char *) vm_map(nullptr, 999);

	strcpy(swap_backed2, "shakespeare.txt"); //evict

	char * spotHolder1 = (char *) vm_map(nullptr, 0);
	char * spotHolder2 = (char *) vm_map(nullptr, 3);

	strcpy(spotHolder1, "data1.bin"); //evict

	strcpy(spotHolder2, "data2.bin"); //evict

	swap_backed[0] = 'a'; //evict

    fork(); //fork here

    swap_backed2[1] = 'b';

    vm_yield(); //switch here

    if (swap_backed[0] == 'a') { //should be true, write happened before forking, also evict
        cout << "Twinning :D" << endl;
    }

    if (swap_backed2[1] = 'b') { //should be false, write happened after forking, also evict
        cout << "Peeking is bad :(" << endl;
    }

    spotHolder1[2] = 'c'; //evict

    exit(EXIT_SUCCESS); //kill child

   	spotHolder2[3] = 'd'; //evict

    if (spotHolder1[2] == 'c') { //evict, also should be false, since change was for other process and swap is private
    	cout << "Stop it, he's already dead..." << endl;
    }

}