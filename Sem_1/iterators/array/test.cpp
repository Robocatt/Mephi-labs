#define _USE_MATH_DEFINES
#include<iostream>
#include<fstream>
#include<cstdlib>
#include<limits>
#include<vector>
#include<queue>
#include<deque>
#include<set>
#include<map>
#include<string>
#include<utility>
#include<cmath>
#include<algorithm>
#include"array.h"
#include <cassert>
#include <random>
#include <utility>
typedef unsigned long long ul;
typedef long long ll;
using namespace std;

std::mt19937 g_random_engine(11);

    template <typename T, typename Generator>
    std::pair<Array<T>, std::vector<T>> GenerateSample(int size, Generator generator) {
        static std::ostream devnull(nullptr);
        std::vector<T> vector(size);
        Array<T> array(size, devnull);

        for (int i = 0; i < size; ++i) {
            vector[i] = generator();
            array[i] = vector[i];
        }

        return {array, vector};
    }

int main(){
    Array<int> a(5,cout,5);

  
    
    auto array_it = a.begin();
    {
        const auto [array, vector] = GenerateSample<int>(10, g_random_engine);

        int i = 0;
        for (auto x : array) {
            assert(x == vector[i++]);
        }
    }
    

    return 0;
}