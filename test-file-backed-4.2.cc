#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"
#include <stdlib.h>

using namespace std;

int main()
{
    char *filename = (char *) vm_map("shakespeare.txt", 0);

    char *filename2 = (char *) vm_map("shakespeare.txt", 0);

    char *filename4 = (char *) vm_map("submission.txt", 0);

    cout << filename[0] << endl; //read

    cout << filename4[0] << endl; //read and evict

    cout << filename4[1] << endl;

    filename4[2] = 'a';

    pid_t child_pid = fork(); 

    vm_yield();

    filename2[3] = 'b'; //evict

    if (filename2[3] != 'b') { //shouldn't ever be true?
        cout << "Something smells fishy..." << endl;
    }

    filename2[3] = 'c';

    if (child_pid == 0) {
        cout << "Kid" << endl;
    } else {
        cout << "Grown up" << endl;
    }

}