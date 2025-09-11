/*
 * Base thread implementation for RxLogger
 * Provides cross-platform threading abstraction using std::thread
 */
#include "../include/base_thread.h"
#include "../include/logger_util.h"

std::vector<rx_base_thread*> rx_base_thread::_all_threads;
std::mutex rx_base_thread::_threads_mutex;

rx_base_thread::rx_base_thread() : _run_flag(false) {
    std::lock_guard<std::mutex> lock(_threads_mutex);
    _all_threads.push_back(this);
}

rx_base_thread::~rx_base_thread() {
    stop();
    // Ensure thread is joined to avoid std::terminate at program exit
    join();
    
    std::lock_guard<std::mutex> lock(_threads_mutex);
    auto it = std::find(_all_threads.begin(), _all_threads.end(), this);
    if (it != _all_threads.end()) {
        _all_threads.erase(it);
    }
}

void rx_base_thread::start() {
    if (!_run_flag) {
        _run_flag = true;
        try {
            _thread = std::thread([this]() { 
                this->run(); 
                return nullptr; 
            });
        } catch (const std::exception& e) {
            _run_flag = false;
            RX_THROW_COMMON_EXCEPT("std::thread creation failed: " << e.what());
        }
    }
}

void rx_base_thread::stop() {
    _run_flag = false;
}

void rx_base_thread::join() {
    if (_thread.joinable()) {
        _thread.join();
    }
}


void rx_base_thread::join_all_thread() {
    std::lock_guard<std::mutex> lock(_threads_mutex);
    for (auto* thread : _all_threads) {
        if (thread) {
            thread->join();
        }
    }
} 
