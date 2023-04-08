#pragma once
#include <vector>
#include <iostream>
#include <algorithm>
#include <iterator>
typedef long long ll;

template<typename T>
class FlattenIterator : public std::iterator<std::random_access_iterator_tag, T>{

    public:
        using difference_type = std::ptrdiff_t;

        FlattenIterator<T>(std::vector<std::vector<T>>& arr_, ll index_, std::vector<ll>& prefs_) : arr(arr_), prefs(prefs_) {
            index = index_;
        }

        FlattenIterator<T>& operator++() {
            if (index < prefs.back()){
                index++;
            }
            return (*this);
        }

        FlattenIterator<T> operator++(int) { 
            FlattenIterator<T> temp = *this;
            ++(*this);
            return temp;
        }

        FlattenIterator<T>& operator--() {
            if (index - 1 > -1){
                index--;
            }
            return (*this);
        }

        difference_type operator-(const FlattenIterator<T>& it) const {
            return this -> index - it.index;
        }

        FlattenIterator<T> operator-(const difference_type& diff) const {
            ll new_idx = index - diff;
            if (new_idx > prefs.back()) {
                new_idx = prefs.back();
            }
            return FlattenIterator<T>(arr, new_idx, prefs);
        }

        FlattenIterator<T> operator+(const difference_type& val) const {
            ll new_idx = index + val;
            if (new_idx > prefs.back()){
                new_idx = prefs.back();
            }
            return FlattenIterator<T>(arr, new_idx, prefs);
        }

        FlattenIterator<T>& operator+=(const difference_type& val) {
            if (index + val <= prefs.back()){
                index += val;
            }
            else{
                index = prefs.back();
            }
            return (*this);
        }

        FlattenIterator<T>& operator-=(const difference_type& val) {
            if (index - val >= 0){
                index -= val;
            }
            else{
                index = 0;
            }
            return (*this);
        }

        FlattenIterator<T>& operator=(const FlattenIterator<T>& it) {
            this -> arr = it.arr;
            this -> index = it.index;
            this -> prefs = it.prefs;

            return (*this);
        }

        T& operator[](const difference_type& val) const {
            return *(*this + val);
        }

        bool operator==(const FlattenIterator<T>& it) const {
            return this -> index == it.index;
        }

        bool operator!=(const FlattenIterator<T>& it) const {
            return !(*this == it);
        }

        bool operator< (const FlattenIterator<T>& it) const {
            return this -> index < it.index;
        }

        bool operator> (const FlattenIterator<T>& it) const {
            return (it < *this);
        }

        bool operator>=(const FlattenIterator<T>& it) const {
            return (it < *this) || (*this == it);
        }

        bool operator<=(const FlattenIterator<T>& it) const {
            return (*this < it) || (*this == it);
        }

        T& operator*() const {
            if (index >= prefs.back()) {
                return arr.back().back();
            }

            std::vector<ll>::iterator outPos = std::upper_bound(prefs.begin(), prefs.end(), index) - 1;
            ll innerPos = index - (*outPos);

            return arr[outPos - prefs.begin()][innerPos];
        }

        friend FlattenIterator<T> operator+(const difference_type val, const FlattenIterator<T>& it) {
            return it + val;
        }

    private:
        std::vector<std::vector<T>>& arr;
        ll index;
        std::vector<ll>& prefs;

};

template<typename T>
class FlattenedVector {
    
    public:
        FlattenedVector (std::vector<std::vector<T>>& arr_) : arr(arr_){
            Size = arr.size(); 
            prefixes.push_back(0);
            for (size_t i = 1; i < Size + 1; i++) {
                prefixes.push_back((long long)((prefixes[i - 1] + arr[i - 1].size())));
            }
        }

        FlattenIterator<T> begin() {
            return FlattenIterator<T>(arr, 0, prefixes);
        }

        FlattenIterator<T> end() {
            return FlattenIterator<T>(arr, prefixes.back(), prefixes);
        }

    private:
        std::vector<std::vector<T>>& arr;
        size_t Size;
        std::vector<ll> prefixes;
};
