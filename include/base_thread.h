#ifndef __RX_BASE_THREAD_H__
#define __RX_BASE_THREAD_H__

#include "logger_def.h"

namespace rxlogger {

// Base thread class for rxlogger
class rx_base_thread {
public:
    rx_base_thread();
    virtual ~rx_base_thread();
    
    void start();
    void stop();
    void join();
    
    virtual void* run() = 0;
    
    bool get_run_flag() const { return _run_flag; }
    std::thread::id get_thread_id() const { 
        return _thread.get_id(); 
    }

    static void join_all_thread();

protected:
    volatile bool _run_flag;
    std::thread _thread;
    static std::vector<rx_base_thread*> _all_threads;
    static std::mutex _threads_mutex;
};

// Singleton pattern template (Meyers' Singleton - C++11 thread-safe)
// Renamed to avoid collision with other frameworks
template<typename T>
class rx_base_singleton {
public:
    static T* get_instance() {
        static T instance;
        return &instance;
    }
    
    // Delete copy constructor and assignment operator
    rx_base_singleton(const rx_base_singleton&) = delete;
    rx_base_singleton& operator=(const rx_base_singleton&) = delete;
    
protected:
    rx_base_singleton() = default;
    ~rx_base_singleton() = default;
};

} // namespace rxlogger

#endif 
