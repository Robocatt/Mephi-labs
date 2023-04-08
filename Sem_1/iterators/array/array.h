#pragma once
#include <iostream>
#include <iterator>
template<typename T>
class ArrayIterator : public std::iterator<std::random_access_iterator_tag, T>{

    public:
    using difference_type = std::ptrdiff_t;
    ArrayIterator(T* ptr) : cur_ptr(ptr)  {}

    ArrayIterator& operator++(){//prefix
        ++cur_ptr;
        return *this;
    }
    ArrayIterator operator++(int) { //postfix
        return ArrayIterator(cur_ptr++);
    }
    ArrayIterator& operator--(){//prefix
        --cur_ptr;
        return *this;
    }
    ArrayIterator operator--(int) { //postfix
        return ArrayIterator(cur_ptr--);
    }

    ArrayIterator operator+(const difference_type val) const 
    {//postfix
        return ArrayIterator (cur_ptr + val);
    }
    
    ArrayIterator operator+(const ArrayIterator& other) const
    {
        return ArrayIterator(*this + other.cur_ptr);
    }

    ArrayIterator operator-(const difference_type val) const
    {
        return ArrayIterator (cur_ptr - val);
    }

    difference_type operator-(const ArrayIterator& other) const
    {
        return std::distance(other.cur_ptr, cur_ptr);
    }

    ArrayIterator& operator+=(const difference_type other){
        cur_ptr += other;
        return *this;
    }

    ArrayIterator& operator-=(const difference_type other){
        cur_ptr -= other;
        return *this;
    }

    bool operator<(const ArrayIterator& a) const
    {
        return cur_ptr < a.cur_ptr;
    }

    bool operator>(const ArrayIterator& a) const 
    {
        return !(operator<(a));
    }

    bool operator<=(const ArrayIterator& a) const
    {
        return !(operator>(a));
    }

    bool operator>=(const ArrayIterator& a) const
    {
        return !(operator<(a));
    }

    bool operator== (const ArrayIterator& a) const
    {
        return a.cur_ptr == cur_ptr;
    };
    bool operator!= (const ArrayIterator& a) const
    {
        return a.cur_ptr != cur_ptr;
    }; 
    
    T& operator*() const{
        return *cur_ptr;
    }

    T& operator*() {
        return *cur_ptr;
    }

    T& operator[](size_t val) const
    {
        return cur_ptr[val];
    }

    T *operator->() 
    {

        return *(&this->cur_ptr);
    }

    private:
    T* cur_ptr;

};


template<typename T>
    ArrayIterator<T> operator+(const int val, const ArrayIterator<T>& it){
        return it + val;
    }
    

template<typename T>
class Array {
    friend class iterator;
    friend std::ostream& operator<<(std::ostream& ostream, const Array<T>& array){
        if (array.Size() == 0){
            ostream << "Result Array's capacity is " << array.Capacity() << " and size is " << 0;
        }else{
            ostream << "Result Array's capacity is " << array.Capacity() << " and size is " << array.Size()
                << ", elements are: ";
            for (size_t i = 0; i < array.Size() - 1; ++i){
                ostream << *(array.Elements_ + i) << ", ";
            }
            ostream << *(array.Elements_ + array.Size() - 1);
        }
        return ostream;
    }
public:
    Array(std::ostream& ostream) : Ostream_(ostream) {
        Elements_ = new T[2];
        Size_ = 0;
        Capacity_ = 2;
        Ostream_ << "Constructed. " << *this << "\n";

    }

    Array(const Array<T>& array) : Ostream_(array.Ostream_) {
        Size_ = array.Size();
        Capacity_ = array.Capacity();
        Elements_ = new T[Capacity()];
        for (size_t i = 0; i < Size(); ++i){
            Elements_[i] = array.Elements_[i];
        }
        Ostream_ << "Constructed from another Array. " << *this << "\n";//!
    }

    Array(size_t size, std::ostream& ostream = std::cout, T defaultValue = T()) : Ostream_(ostream){
        Size_ = size;
        Capacity_ = 2 * size;
        Elements_ = new T[Capacity()];
        for (size_t i = 0; i < size; ++i){
            Elements_[i] = defaultValue;
        }
        Ostream_ << "Constructed with default. "<< *this << "\n";//!

    }

    ~Array() {
        delete[] Elements_;
        Ostream_ << "Destructed " << Size_ << "\n";
    }

    size_t Size() const {
        return Size_;
    }

    size_t Capacity() const {
        return Capacity_;
    }

    void Reserve(size_t newCapacity) {
        if (newCapacity > Capacity()){
            T* Elems_ = new T[newCapacity];
            for (size_t i = 0; i < Size(); ++i){
                Elems_[i] = Elements_[i];
            }
            delete[] Elements_;
            Elements_ = Elems_;
            Capacity_ = newCapacity;
        }
    }

    void Resize(size_t newSize) {
        if (newSize > Capacity()){
            Reserve(newSize);
        }
        Size_ = newSize;
    }

    void PushBack(T value = 0) {
        if (Size() == Capacity()){
            Reserve(Capacity() * 2);
        }
        *(Elements_ + int(Size())) = value;
        Size_++; 
    }

    void PopBack() {
        Size_--;
    }

    const T& operator [](const size_t i) const {
        return *(Elements_ + i);
    }

    T& operator [](const size_t i) {
        return *(Elements_ + i);
    }

    explicit operator bool() const {
        return Size() != 0;
    }

    bool operator <(const Array<T>& it) const {
        for (size_t i = 0; i < std::min(Size(), it.Size()); ++i){
            if(Elements_[i] > it.Elements_[i]){
                return false;
            }else if(Elements_[i] < it.Elements_ [i])
                return true;
        }
        return false;
    }

    bool operator >(const Array<T>& it) const {
        return !Array::operator<(it);
    }

    bool operator !=(const Array<T>& it) const {
        for (size_t i = 0; i < std::min(Size(), it.Size()); ++i){
            if (Elements_[i] != it.Elements_ [i]){
                return true;
            }
        }
        return false;
    }

    bool operator ==(const Array<T>& it) const {
        return !Array::operator!=(it);
    }

    bool operator <=(const Array<T>& it) const {
        for (size_t i = 0; i < std::min(Size(), it.Size()); ++i){
        if (Elements_[i] > it.Elements_[i]){
            return false;
        }else if(Elements_[i] < it.Elements_[i])
            return true;
        }
        return true;
    }

    bool operator >=(const Array<T>& it) const {
        return !Array::operator<=(it);
    }

    Array<T>& operator <<(const T& value) {
        PushBack(value);
        return *this;
    }

    Array<T>& operator <<(const Array<T>& it) {
        for (size_t i = 0; i < it.Size(); ++i){
            PushBack(it.Elements_[i]);
        }
        return *this;
    }

    bool Insert(size_t pos, const T& value) {
        if(pos > Size()){
            return false;
        }else{
            if(Size() == Capacity()){
                Reserve(Capacity() * 2);
            }
            if(pos == Size()){
                PushBack(value);
            }else{
                if(pos != 0){
                    pos--;
                }
                T mem1 = Elements_[pos];
                T mem2;
                Elements_[pos] = value;
                for(size_t i = pos + 1; i < Size(); i++){
                    mem2 = Elements_[i];
                    Elements_[i] = mem1;
                    mem1 = mem2;
                }
                Size_++;
            }
            
            return true;
        }

    }

    bool Erase(size_t pos) {
        if(pos > Size()){
            return false;
        }else{
            if(pos == Size()){
                PopBack();
            }else{
                for(size_t i = pos; i < Size() - 1; ++i){
                    Elements_[i] = Elements_[i + 1];
                }
                Size_--;
            }
            return true;
        }
    }

    void clear(){
        delete[] Elements_;
        Elements_ = new T[Size()];
    }

    bool operator < (const Array<T>& array){
        for (size_t i = 0; i < std::min(Size(), array.Size()); ++i){
            if(Elements_[i] > array.Elements_[i]){
                return false;
            }else if(Elements_[i] < array.Elements_ [i])
                return true;
        }
        return false;
    }

    ArrayIterator<T> begin() const{
        return ArrayIterator(Elements_);
    }
    ArrayIterator<T> end() const {
        return ArrayIterator(Elements_ + Size_);
    }

    ArrayIterator<T> begin(){
        return ArrayIterator(Elements_);
    }
    ArrayIterator<T> end() {
        return ArrayIterator(Elements_ + Size_);
    }

   

private:
    T* Elements_;
    size_t Size_;
    size_t Capacity_;
    std::ostream& Ostream_;
};
