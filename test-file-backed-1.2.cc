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

    cout << filename[0] << endl; //read

    cout << filename2[0] << endl; //read, but should not evict

    if (filename[0] == filename2[0]) { //double read, should not evict anything
        cout << "d same" << endl;
    }

    cout << filename3[0] << endl; //out of arena

    cout << filename4[0] << endl; //read, should evict

    if (filename[0] == filename4[0]) { //double read, should evict twice, shouldnt cout?
        cout << "kill me" << endl;
    }

    if (filename4[0] == filename2[0]) { //double read, should evict once, shouldnt cout?
        cout << "jinkies" << endl;
    }

    cout << filename5[0] << endl; //another out of arena

    cout << filename6[0] << endl; //read, evict

    if (filename6[0] == filename[0]) {//double read, should evict once, should cout
        cout << "2 shakespeares" << endl;
    }

    cout << filename7 << endl; //read, no evict, not out of arena
}