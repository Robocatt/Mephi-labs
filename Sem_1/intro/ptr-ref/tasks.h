#pragma once
#define _USE_MATH_DEFINES
#include<iostream>
#include<fstream>
#include<vector>
#include<queue>
#include<deque>
#include<set>
#include<map>
#include<string>
#include<utility>
#include<cmath>
#include<algorithm>
#include<limits>
#include<cstring>
typedef unsigned long long ul;
typedef long long ll;


namespace NPointers {
    void Increment(int *a);

    int Multiply(int a, int b, bool *f);

    int ScalarProduct(const int *m1, const int *m2, int size);

    int SizeOfMaximumSquareOfCrosses(const char *array, int s1, int s2);
    
    long long* MultiplyToLongLong(const int a, const int b);

}

namespace NReferences {

    void MultiplyInplace(int &a, const int b);

    int CompareArraysByAverage(const int *m1, const int size, int *m2);

}
