#pragma once

#include <atomic>

class SharedMutex {
public:
    void lock() {
        int zero = 0;
        while(!status_.compare_exchange_strong(zero, -1)){
            zero = 0;
        }
    }

    void unlock() {
        status_.store(0);
    }

    void lock_shared() {
        int st;
        do{
            do{
                st = status_.load();
            }while(st == -1);
        }while(!status_.compare_exchange_strong(st, st + 1));
    }

    void unlock_shared() {
        status_.fetch_sub(1);
    }
private:
    std::atomic<int> status_ = 0;
};
