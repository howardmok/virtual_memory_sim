// Catches bugs Nh

#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using namespace std;

int main()
{
    char *filename = (char *) vm_map("shakespeare.txt", 0);

    char *filename2 = (char *) vm_map("shakespeare.txt", 0);

    char *filename3 = (char *) vm_map("shakespeare.txt", 1);

    char *filename4 = (char *) vm_map("submission.txt", 0);

    char *filename5 = (char *) vm_map("submission.txt", 1);

    char *filename6 = (char *) vm_map("shakespeare2.txt", 0);

    char *filename7 = (char *) vm_map("shakespeare2.txt", 1);

    filename[0] = 'a'; //write

    if (filename[0] == 'a') { //should be written back
        cout << "we did it" << endl;
    }

    if (filename2[0] == 'a') { //should be the same as shared file
        cout << "sharing is caring" << endl;
    }

    filename2[0] = 'b';

    if (filename2[0] == 'b') { //should be written back
        cout << "we did it again" << endl;
    }

    if (filename[0] == 'b') { //should be the same as shared file
        cout << "sharing is very much so caring" << endl;
    }

    filename3[0] = 'c'; //out of arena

    filename4[0] = 'd'; //should evict on this one

    filename5[0] = 'e'; //out of arena

    filename[0] = 'f'; //should evict on this one

    filename[0] = 'f'; //shouldn't have to write it back cause we just did it?

    filename6[0] = 'g'; //evict

    filename7[0] = 'h';
    
}