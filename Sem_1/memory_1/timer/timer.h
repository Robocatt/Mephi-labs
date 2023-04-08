#include <chrono>
#include <sstream>
#include <iostream>


class Timer {
    public:
        Timer(std::chrono::seconds dr_) : dr(dr_) {
            start = std::chrono::steady_clock::now();
        } 

        bool Expired() const{
            std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
            return (now - start  > dr ? true : false);
        }

    private:
        std::chrono::seconds dr;
        std::chrono::steady_clock::time_point start;
};

class TimeMeasurer {
    public:
    TimeMeasurer(std::ostream& ostream) : Ostream_(ostream) {
        cl = std::chrono::steady_clock::now();
    }
    public:
    ~TimeMeasurer(){
            std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
            std::chrono::nanoseconds elapsedns = now - cl;  
            std::chrono::milliseconds elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(elapsedns);
            Ostream_ << "Elapsed time: " << elapsed.count() << std::endl;
    }
    private:
        std::chrono::steady_clock::time_point cl;
        std::ostream& Ostream_;
};
