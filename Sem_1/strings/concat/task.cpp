#include "task.h"

char* concat(const char* lhs, const char* rhs) {
    char* buff = new char[1000];
    int i = 0;
    while(*(lhs + i) != '\0'){
        buff[i] = *(lhs + i);
        i++;
    }
    int j = 0;
    while(*(rhs + j) != '\0'){
        buff[i] = *(rhs + j);
        i++;
        j++;
    }
    buff[i] = '\0';
    return buff;
}
