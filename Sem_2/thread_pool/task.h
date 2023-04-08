#include <thread>
#include <mutex>
#include <vector>
#include <queue>
#include <functional>
#include <condition_variable>
#include <atomic>
#include <stdexcept>
#include <cassert>
#include <iostream>
#include <chrono>

/*
 * Требуется написать класс ThreadPool, реализующий пул потоков, которые выполняют задачи из общей очереди.
 * С помощью метода PushTask можно положить новую задачу в очередь
 * С помощью метода Terminate можно завершить работу пула потоков.
 * Если в метод Terminate передать флаг wait = true,
 *  то пул подождет, пока потоки разберут все оставшиеся задачи в очереди, и только после этого завершит работу потоков.
 * Если передать wait = false, то все невыполненные на момент вызова Terminate задачи, которые остались в очереди,
 *  никогда не будут выполнены.
 * После вызова Terminate в поток нельзя добавить новые задачи.
 * Метод IsActive позволяет узнать, работает ли пул потоков. Т.е. можно ли подать ему на выполнение новые задачи.
 * Метод GetQueueSize позволяет узнать, сколько задач на данный момент ожидают своей очереди на выполнение.
 * При создании нового объекта ThreadPool в аргументах конструктора указывается количество потоков в пуле. Эти потоки
 *  сразу создаются конструктором.
 * Задачей может являться любой callable-объект, обернутый в std::function<void()>.
 */

class ThreadPool {
public:
    ThreadPool(size_t threadCount) : active_(1), terminate_(0){
        for (size_t i = 0; i < threadCount; ++i) {
            threads_.emplace_back(std::bind(&ThreadPool::Worker, this));
        }
    }

    void PushTask(const std::function<void()>& task) {
        std::unique_lock<std::mutex> lock(mutex_);
        if (terminate_){
            throw std::runtime_error("Was terminated before");
        }
        tasks_.push(task);
        if(!tasks_.empty()){
            cv_.notify_one();
        }
    }

     void Terminate(bool wait) {
        terminate_.store(true);
        if (wait) {          
            cv_.notify_all();
            for (auto& thread : threads_) {
                thread.join();
            }
            active_.store(false); 
        }else{
            active_.store(false);
            {
                std::unique_lock<std::mutex> lock(mutex_);
                std::queue<std::function<void()>> emp = {};
                std::swap(emp, tasks_);
            }
            cv_.notify_all();
            for (auto& thread : threads_) {
                thread.join();
            }
        }
    }

    bool IsActive() const {
        return active_.load();
    }

    size_t QueueSize() const {
        std::unique_lock<std::mutex> lock(mutex_);
        return tasks_.size();
    }


private:

    void Worker(){
        while (true) {
            std::unique_lock<std::mutex> lock(mutex_);
            cv_.wait(lock, [this]() { return terminate_ || !tasks_.empty(); });
            if (terminate_ && tasks_.empty()) {
                return;
            }
            std::function<void()> task = tasks_.front();
            tasks_.pop();
            lock.unlock();
            task();
        }
    }

    std::atomic<bool> active_;
    std::atomic<bool> terminate_;
    std::vector<std::thread> threads_;
    std::queue<std::function<void()>> tasks_;
    mutable std::mutex mutex_;
    std::condition_variable cv_;
    
};