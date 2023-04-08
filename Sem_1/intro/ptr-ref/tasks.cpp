#include "tasks.h"

void NPointers::Increment(int *a){
    (*a)++;
}

int NPointers::Multiply(int a, int b, bool *f){
    const int maxInt = std::numeric_limits<int>::max();
    if( maxInt / abs(b) >= abs(a)){
        *f = false;
        return a * b;
    }
    *f = true;
    return -1;
}

int NPointers::ScalarProduct(const int *m1, const int *m2, int size){
    int res = 0;
    for(int i = 0; i < size; i++){
        res += m1[i] * m2[i];
    }
    return res;
}

int NPointers::SizeOfMaximumSquareOfCrosses(const char *array, int s1, int s2){
    std::vector<std::vector<int>> arr(s1);
    for(int i = 0; i < s1; i++){
        for(int j = 0; j < s2; j++){
            arr[i].push_back(*(array + ((i * s2) + j)));
        }
    } 
    int DP[s1][s2];
    std::memset(DP, sizeof(DP), 0);
    int maxSqMatSize = 0;
    for (int i = 0 ; i < s1; i++){
        for (int j = 0 ; j < s2 ; j++){
          if (i == 0 || j == 0)
            DP[i][j] = 1;
          else{
            if (arr[i][j] == arr[i-1][j] && arr[i][j] == arr[i][j-1] && arr[i][j] == arr[i-1][j-1] )
               DP[i][j] = std::min(std::min(DP[i-1][j], DP[i][j-1]), DP[i-1][j-1] ) + 1;
            else DP[i][j] = 1;
         }
        maxSqMatSize = std::max(maxSqMatSize, DP[i][j]);
        }
   }
   return maxSqMatSize;
}

long long* NPointers::MultiplyToLongLong(const int a, const int b){
    ll* res = new long long(a);
    *res *= b;
    return res;
}

void NReferences::MultiplyInplace(int &a, const int b){
   a *= b;
}

int NReferences::CompareArraysByAverage(const int *m1, const int size, int *m2){
    double s1 = 0;
    for(int i = 0; i < size; i++){
        s1 += (*(m1 + i));

    }
    double s2 = 0;
    for(int i = 0; i < 5; i++){
        s2 += (*(m2 + i));
    }
    s1 /= size;
    s2 /= 5.0;
    if(s1 == s2){
        return 0;
    }else{
        return (s1 > s2 ? 1 : -1);
    }

}