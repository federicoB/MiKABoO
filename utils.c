#include "utils.h"
#include "listx.h"

void memcopy(void* src, void* dst, size_tt size){
    //while there are bytes to copy
    while (size--){
        //copy current byte and then increment both pointers
        *(char*) dst++ = *(char*) src++;
    }
}