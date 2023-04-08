#pragma once

#include <cstdint>
#include <mutex>
#include <shared_mutex>
#include <set>
#include <atomic>
#include <vector>
#include <iostream>

template<typename T>
struct TNode{
    T data;
    TNode* next;
    TNode* prev;
    std::mutex local_mutex;

    TNode(const T& value = T()) : data(value), next(nullptr), prev(nullptr) {}

};

/*
 * Потокобезопасный связанный список.
 */
template<typename T>
class ThreadSafeList {
public:
    /*
     * Класс-итератор, позволяющий обращаться к элементам списка без необходимости использовать мьютекс.
     * При этом должен гарантироваться эксклюзивный доступ потока, в котором был создан итератор, к данным, на которые
     * он указывает.
     * Итератор, созданный в одном потоке, нельзя использовать в другом.
     */
    class Iterator {
    public:
        using pointer = T*;
        using value_type = T;
        using reference = T&;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::bidirectional_iterator_tag;

        Iterator(TNode<T>* ptr = nullptr) : cur_ptr(ptr) {}
        Iterator(const Iterator& it){
            cur_ptr = (it.cur_ptr);
        }
        
        Iterator& operator=(const Iterator& other) = default;
        Iterator& operator=(TNode<T>* ptr){
            this->cur_ptr=ptr;
            return(*this);
        }
        

        ~Iterator(){}

        operator bool()const{
            return cur_ptr ? true : false;
        }

        T& operator *() {
            cur_ptr->local_mutex.lock();

            return cur_ptr->data;
        }
        
        T operator *() const {
            cur_ptr->local_mutex.lock();
            
            return cur_ptr->data;
        }

        T* operator ->() {
            std::lock_guard guard(cur_ptr->local_mutex);
            return &(cur_ptr->data);
        }
        
        const T* operator ->() const {
            std::lock_guard guard(cur_ptr->local_mutex);
            
            return (&this->cur_ptr);
        }

        Iterator& operator ++() {//prefix
            cur_ptr->local_mutex.try_lock();
            
            auto x = cur_ptr;
            cur_ptr = cur_ptr->next;
            x->local_mutex.unlock();
            return *this;
        }
        
        Iterator operator ++(int) {//postfix
            cur_ptr->local_mutex.try_lock();
            Iterator copy(*this);
            auto x = cur_ptr;
            cur_ptr = cur_ptr->next;
            x->local_mutex.unlock();
            return copy;
        }
        
        Iterator& operator --() {//prefix
            cur_ptr->local_mutex.try_lock();
            cur_ptr = cur_ptr->prev;
            cur_ptr->next->local_mutex.unlock();
            return *this;
        }

        Iterator operator --(int) {//postfix
            cur_ptr->local_mutex.try_lock();
            Iterator copy(*this);
            cur_ptr = cur_ptr->prev;
            cur_ptr->next->local_mutex.unlock();
            return copy;
        }

        bool operator ==(const Iterator& rhs) const {
            return rhs.cur_ptr == cur_ptr;
        }

        bool operator !=(const Iterator& rhs) const {
            std::shared_lock guard(iterator_mutex);
            std::shared_lock guard2(rhs.iterator_mutex);
            return rhs.cur_ptr != cur_ptr;
        }

    friend class ThreadSafeList;
    private:
        bool is_locked;
        TNode<T>* cur_ptr;
        mutable std::shared_mutex iterator_mutex;
    };

    ThreadSafeList() : head(nullptr), tail(nullptr) {
        end_node = new TNode<T>();
        end_node->prev=tail;
    }

    /*
     * Получить итератор, указывающий на первый элемент списка
     */
    Iterator begin() {
        std::lock_guard<std::mutex> guard(list_mutex);
        if(head){
            std::lock_guard guard2(head->local_mutex);
        }
        return Iterator(head);
    }

    /*
     * Получить итератор, указывающий на "элемент после последнего" элемента в списке
     */
    Iterator end() {
        std::lock_guard<std::mutex> guard(list_mutex);
        return Iterator(end_node);
    }

    /*
     * Вставить новый элемент в список перед элементом, на который указывает итератор `position`
     */
    void insert(Iterator position, const T& value) {
        std::lock_guard guard(list_mutex);
        TNode<T>* new_node = new TNode<T>(value);
        if(head){
            if(head == position.cur_ptr){
                head -> prev = new_node;
                new_node -> next = head;
                head = new_node;
            }else if(position.cur_ptr != end_node){
                position.cur_ptr->prev->next = new_node;
                new_node->prev=position.cur_ptr->prev;
                position.cur_ptr->prev = new_node;
                new_node->next = position.cur_ptr;
            }else{
                //end
                new_node->next=tail->next;
                new_node->prev=tail;
                tail->next->prev = new_node;
                tail->next = new_node;
            }
        }else{
            new_node->next=end_node;
            head = new_node;
            tail = new_node;
        }
    }

    /*
     * Стереть из списка элемент, на который указывает итератор `position`
     */
    void erase(Iterator position) {
        std::lock_guard guard(list_mutex);
        if(position.cur_ptr == head){
            head->next->prev=nullptr;
            head = head->next;
        }else if(position.cur_ptr){
            position.cur_ptr->prev->next = position.cur_ptr->next;
            position.cur_ptr->next->prev = position.cur_ptr->prev;
        }
    }

private:
    TNode<T>* head;
    TNode<T>* tail;
    TNode<T>* end_node; //after tail
    std::mutex list_mutex;

};
