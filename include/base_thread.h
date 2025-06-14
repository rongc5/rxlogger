#ifndef __BASE_THREAD_H__
#define __BASE_THREAD_H__

#include "logger_def.h"

// 基础线程类
class base_thread {
public:
    base_thread();
    virtual ~base_thread();
    
    void start();
    void stop();
    void join();
    
    virtual void* run() = 0;
    
    bool get_run_flag() const { return _run_flag; }
    pthread_t get_thread_id() const { return _thread_id; }

    static void join_all_thread();

private:
    static void* thread_func(void* arg);

protected:
    volatile bool _run_flag;
    pthread_t _thread_id;
    static vector<base_thread*> _all_threads;
    static mutex _threads_mutex;
};

// 单例模式模板
template<typename T>
class base_singleton {
private:
    static T* _instance;
    static mutex _mutex;

public:
    static T* get_instance() {
        if (_instance == nullptr) {
            lock_guard<mutex> lock(_mutex);
            if (_instance == nullptr) {
                _instance = new T();
            }
        }
        return _instance;
    }
    
    static T* get_instance_ex() {
        return get_instance();
    }
    
    static void set_instance(T* instance) {
        lock_guard<mutex> lock(_mutex);
        if (_instance) {
            delete _instance;
        }
        _instance = instance;
    }
    
    static void destroy() {
        lock_guard<mutex> lock(_mutex);
        if (_instance) {
            delete _instance;
            _instance = nullptr;
        }
    }
};

template<typename T>
T* base_singleton<T>::_instance = nullptr;

template<typename T>
mutex base_singleton<T>::_mutex;

#endif 