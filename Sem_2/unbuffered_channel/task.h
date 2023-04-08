#pragma once

#include <mutex>
#include <condition_variable>
#include <optional>

class TimeOut : public std::exception {
    const char* what() const noexcept override {
            return "Timeout";
    }
};

template<typename T>
class UnbufferedChannel {
public:
    void Put(const T& data) {
        std::unique_lock<std::mutex> lock(mutex_);
        if(!data_){
            data_ = data;
            empty_.notify_one();
        }else{
            while(data_){
                full_.wait(lock);
            }
            if(!data_){
                data_ = data;
                empty_.notify_one();
            }else{
                throw TimeOut();
            }
        }
        block_.wait(lock);
    }

    T Get(std::chrono::milliseconds timeout = std::chrono::milliseconds(0)) {
        std::unique_lock<std::mutex> lock(mutex_);
        if(data_){
            T cur_d = data_.value();
            data_.reset();
            full_.notify_one();
            block_.notify_one();
            return cur_d;
        }else{
            if(empty_.wait_for(lock, timeout) == std::cv_status::no_timeout && data_){
                T cur_d = data_.value();
                data_.reset();
                full_.notify_one();
                block_.notify_one();
                return cur_d;
            }else{
                throw TimeOut();
            }
        }
        throw TimeOut();
    }

private:
    std::mutex mutex_;
    std::condition_variable empty_;
    std::condition_variable full_;
    std::condition_variable block_;
    std::optional<T> data_;

};
