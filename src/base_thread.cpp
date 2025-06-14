/*
 * @Author: zhangming025251 rongc5@users.noreply.github.com
 * @Date: 2025-06-14 18:29:09
 * @LastEditors: zhangming025251 rongc5@users.noreply.github.com
 * @LastEditTime: 2025-06-14 18:35:31
 * @FilePath: /test/logger/src/base_thread.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "../include/base_thread.h"
#include "../include/logger_util.h"

vector<base_thread*> base_thread::_all_threads;
mutex base_thread::_threads_mutex;

base_thread::base_thread() : _run_flag(false), _thread_id(0) {
    lock_guard<mutex> lock(_threads_mutex);
    _all_threads.push_back(this);
}

base_thread::~base_thread() {
    stop();
    
    lock_guard<mutex> lock(_threads_mutex);
    auto it = find(_all_threads.begin(), _all_threads.end(), this);
    if (it != _all_threads.end()) {
        _all_threads.erase(it);
    }
}

void base_thread::start() {
    if (!_run_flag) {
        _run_flag = true;
        int ret = pthread_create(&_thread_id, NULL, thread_func, this);
        if (ret != 0) {
            _run_flag = false;
            THROW_COMMON_EXCEPT("pthread_create failed: " << strerror(ret));
        }
    }
}

void base_thread::stop() {
    _run_flag = false;
}

void base_thread::join() {
    if (_thread_id != 0) {
        pthread_join(_thread_id, NULL);
        _thread_id = 0;
    }
}

void* base_thread::thread_func(void* arg) {
    base_thread* thread = static_cast<base_thread*>(arg);
    if (thread) {
        return thread->run();
    }
    return NULL;
}

void base_thread::join_all_thread() {
    lock_guard<mutex> lock(_threads_mutex);
    for (auto* thread : _all_threads) {
        if (thread) {
            thread->join();
        }
    }
} 