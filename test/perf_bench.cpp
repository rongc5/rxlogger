#include "../include/log_helper.h"
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <atomic>
#include <cstdlib>

using namespace std;

static void log_round_robin(size_t count) {
    for (size_t i = 0; i < count; ++i) {
        switch (i % 5) {
            case 0: LOG_DEBUG("bench msg %zu", i); break;
            case 1: LOG_TRACE("bench msg %zu", i); break;
            case 2: LOG_NOTICE("bench msg %zu", i); break;
            case 3: LOG_WARNING("bench msg %zu", i); break;
            case 4: LOG_FATAL("bench msg %zu", i); break;
        }
    }
}

int main(int argc, char** argv) {
    // args: [count] [threads]
    size_t count = (argc > 1) ? strtoull(argv[1], nullptr, 10) : 10000ULL;
    size_t threads = (argc > 2) ? strtoull(argv[2], nullptr, 10) : 1ULL;

    cout << "RxLogger perf bench: count=" << count << ", threads=" << threads << endl;

    // Use dedicated bench config
    LOG_INIT("perf_logger.conf");

    // warm up
    for (int i = 0; i < 100; ++i) {
        LOG_DEBUG("warmup %d", i);
    }
    this_thread::sleep_for(chrono::milliseconds(50));

    auto t0 = chrono::high_resolution_clock::now();

    if (threads <= 1) {
        log_round_robin(count);
    } else {
        vector<thread> ths;
        size_t per = count / threads;
        size_t rem = count % threads;
        for (size_t i = 0; i < threads; ++i) {
            size_t n = per + (i < rem ? 1 : 0);
            ths.emplace_back([n]() { log_round_robin(n); });
        }
        for (auto &th : ths) th.join();
    }

    auto t1 = chrono::high_resolution_clock::now();
    auto dur_us = chrono::duration_cast<chrono::microseconds>(t1 - t0).count();
    double per_log_us = dur_us / static_cast<double>(count);
    double throughput = (count * 1e6) / (dur_us ? dur_us : 1);
    cout << "produce time: " << dur_us << " us, per log: " << per_log_us << " us, throughput: " << throughput << " logs/s" << endl;

    // wait flush
    this_thread::sleep_for(chrono::milliseconds(500));

    cout << "done. logs under test/logs with prefix perf_bench.*" << endl;
    return 0;
}

