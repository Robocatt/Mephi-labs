#include "task.h"

void ReverseList(std::list<int>& l) {
    int ptr = 0;
    const int size = l.size();
    int* x = new int [l.size()] ;
    auto it = l.end();
    --it;
    for (; it != l.begin(); --it){
        x[ptr++] = *(it);
    }
    x[ptr] = *(it);
    l.clear();
    for (int i = 0; i < size; ++i){
        l.push_back(x[i]);
    }
}