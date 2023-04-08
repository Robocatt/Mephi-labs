#include "task.h"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <vector>
PrimeNumbersSet::PrimeNumbersSet(){
    PrimeNumbersSet::primes_ = {};
    PrimeNumbersSet::nanoseconds_under_mutex_ = 0;
    PrimeNumbersSet::nanoseconds_waiting_mutex_ = 0;
}

bool PrimeNumbersSet::IsPrime(uint64_t number) const{
    set_mutex_.lock();
    bool f = (find(primes_.begin(), primes_.end(), number) == primes_.end() ? false : true );
    set_mutex_.unlock();
    return f;
}

uint64_t PrimeNumbersSet::GetNextPrime(uint64_t number) const{
    set_mutex_.lock();
    uint64_t num = *find(primes_.begin(), primes_.end(), number);
    uint64_t maxx =  *primes_.rbegin();
    set_mutex_.unlock();
    if(num == maxx){
        throw std::invalid_argument("Don't know next prime after limit\n");
    }
    set_mutex_.lock();
    uint64_t next = *(++find(primes_.begin(), primes_.end(), number));
    set_mutex_.unlock();
    return next;
}

void PrimeNumbersSet::AddPrimesInRange(uint64_t from, uint64_t to){
    std::vector<bool> prime(to + 1, true);
    
    prime[0] = prime[1] = false; 
    for(uint64_t i = 2; i * i <= to; ++i){
        if(prime[i]){
            for(int j = i * i; j * j <= to; j += i){
                prime[j] = false;
            }
        }
    }

    for(int i = 2; i * i <= to; ++i){
        if(prime[i]){
            for(int j = std::max(i * i, ((int(from) - i + 1) / i * i)) ; j <= int(to); j += i){
                prime[j] = false;
            }
        }
    }

    std::vector<int> all;
    for(uint64_t i = from; i <= to; ++i){
        if(prime[i]){
            all.push_back(i);
        }
    }
    
    std::chrono::time_point startTimewaitingMutex = std::chrono::high_resolution_clock::now();
    set_mutex_.lock();
    std::chrono::time_point endTimewaitingMutex = std::chrono::high_resolution_clock::now();
    nanoseconds_waiting_mutex_ +=  std::chrono::duration_cast<std::chrono::nanoseconds>(endTimewaitingMutex -  startTimewaitingMutex).count();
    
    std::chrono::time_point startTimeUnderMutex = std::chrono::high_resolution_clock::now();
    for(auto i : all){
        primes_.insert(i);
    }
    std::chrono::time_point endTimeUnderMutex = std::chrono::high_resolution_clock::now();
    set_mutex_.unlock();
    nanoseconds_under_mutex_ +=  std::chrono::duration_cast<std::chrono::nanoseconds>(endTimeUnderMutex -  startTimeUnderMutex).count();
    

            
        
    
}


size_t PrimeNumbersSet::GetPrimesCountInRange(uint64_t from, uint64_t to) const{
    uint64_t dst;
    set_mutex_.lock();
    dst = std::max(1, int(std::distance(primes_.lower_bound(from), primes_.lower_bound(to))));
    set_mutex_.unlock();
    return dst;
}

uint64_t PrimeNumbersSet::GetMaxPrimeNumber() const{
    set_mutex_.lock();
    std::atomic<uint64_t> maxx = 0;
    if(primes_.size() != 0){
        maxx = *primes_.rbegin();
    }
    set_mutex_.unlock();
    return maxx;
}

std::chrono::nanoseconds PrimeNumbersSet::GetTotalTimeWaitingForMutex() const{
    return std::chrono::nanoseconds(nanoseconds_waiting_mutex_);
}

std::chrono::nanoseconds PrimeNumbersSet::GetTotalTimeUnderMutex() const{
    return std::chrono::nanoseconds(nanoseconds_under_mutex_);
}