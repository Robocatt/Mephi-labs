#include "tasks.h"



int NOuter::NInner::DoSomething(int lhs, int rhs) {
    return lhs + rhs;
}

int NOuter::DoSomething(int lhs, int rhs) {
    return lhs - rhs;
}

int NOverload::ProcessTwoArgs(int lhs, int rhs) {
    return lhs + rhs;
}

char NOverload::ProcessTwoArgs(char lhs, char rhs) {
    return char(std::max(int(lhs), int(rhs)));
}

int NOverload::ProcessTwoArgs(int lhs, char rhs) {
    return lhs - rhs;
}

unsigned int NOverflow::WithOverflow(int lhs, int rhs) {
    return lhs + rhs;
}

uint64_t NOverflow::WithOverflow(int64_t lhs, int64_t rhs) {
    return lhs - rhs;
}

int NLoop::SumInRange(const int lhs, const int rhs) {
    int summ = 0;
    for(int i = lhs; i < rhs; i++){
        summ += i;
    }
    return summ;
}

int NLoop::CountFixedBitsInRange(const int from, const int to, const int bitsCnt) {
    long long summ = 0;
    for(int i = from; i < to; i++){
        int x = i, curones = 0;
        while(x > 0){
            if(x % 2 == 1){
                curones++;
            }
            x /=2;
        }
        if(curones == bitsCnt){
            summ++;
        }
    }
    return summ;
}

double NMath::ComputeMathFormula(const double arg) {
    return fabs((sin(arg) / 2.0 + cos(arg)) * (sin(arg) / 2.0 + cos(arg)) + tan(arg) * atan(arg));
}

bool NMath::IsPositive(int arg) {
    return (arg > 0);
}

int NRecursion::CalculateFibonacci(const int arg) {
    int sum = 1, a = 0;
    if(arg  ==  1){
        return 1;
    }
    if(arg > 1){
        for(int i = 2;i <= arg; i++){
            sum += a;
            a = sum - a;
        }
        return sum;
    }
    
}
