#pragma once
#include<cstdint>
#include<cmath>
class RangeIterator{
    public:
    RangeIterator(int64_t n, int64_t first_, int64_t step_) : cur(n), first(first_), step(step_) {}

    bool operator<(const RangeIterator& it){
        return cur < it.cur;
    }

    bool operator!=(const RangeIterator& it){
        return it.cur != cur;
    }

    bool operator==(const RangeIterator& it){
        return it.cur == cur;
    }

    RangeIterator operator++(int){//postfix
        RangeIterator copy(*this);
        operator++();
        return copy;
    }

    RangeIterator operator++(){//prefix
        cur ++;
        return *this;
    }

    int64_t operator*(){
        return first + step * cur;
    }

    private:
        int64_t cur;
        int64_t first;
        int64_t step;
};

class Range {
    public:
        Range(int64_t finish_) : start(0), finish(finish_), step(1) {}

        Range(int64_t start_, int64_t finish_) : start(start_), finish(finish_), step(1) {}

        Range(int64_t start_, int64_t finish_, int64_t step_) : start(start_), finish(finish_), step(step_) {}

        int64_t Size() const{
            return ceil((finish - start) / double(step));
        }

        RangeIterator begin(){
            return RangeIterator(0, start, step);
        }

        RangeIterator end(){
            return RangeIterator(Size(), start, step);
        }


        RangeIterator begin() const{
            return RangeIterator(0, start, step);
        }

        RangeIterator end() const{
            return RangeIterator(Size(), start, step);
        }

        

    private:
        int64_t start;
        int64_t finish;
        int64_t step;

};
