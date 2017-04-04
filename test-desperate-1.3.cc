#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using namespace std;

int main() {
    // fork right at the beginning. should do nothing.
    cout << "FOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOORRRRRRRRRKING________" << endl;
    fork();

    cout << "YIIIIIIIIIIIIIIIIIIIIIIEEEEEEEEEEEEEEEEEEELDING-________" << endl;
    vm_yield();

    cout << "swaaaaaaaaaaaaaaap file" << endl;
    char* swap_backed = (char *) vm_map(nullptr, 12);

    cout << "dirrtyyyiinnnng swappppp file ___________________" << endl;
    strcpy(swap_backed, "shakespeare.txt");

    cout << "FILEBACK FILE __________________________________" << endl;
    char* file_backed = (char *) vm_map(swap_backed, 0);

    cout << "COUTING_______________________________________" << endl;
    cout << file_backed[0] << endl;

    cout << "YIIIIIIIIIIIIIIIIIIIIIIEEEEEEEEEEEEEEEEEEELDING222222222" << endl;
    vm_yield();

    cout << "FORKINGNGGNGNGNGNGNGNGNGNNGNGNGNGNGN22222222222222222222" << endl;
    fork();

    
    cout << file_backed[0] << endl;
    file_backed[0] = 'A';
	char* file_backed2 = (char *) vm_map(swap_backed, 0);    
	cout << "file_backed2[0] (should be an A): " << file_backed2[0] << endl;
    cout << "exiting" << endl;
    exit(EXIT_SUCCESS);

    char* swap_backed3 = (char *) vm_map(nullptr, 2);
    strcpy(swap_backed3, "data1.bin");
    cout << file_backed[0] << endl;
    cout << swap_backed3[0] << endl;

   	char* file_backed3 = (char *) vm_map(swap_backed3, 0);
   	file_backed2[0] = 'B';

} // end main()