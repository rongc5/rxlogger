#ifndef __LOG_HELPER_H_
#define __LOG_HELPER_H_

#include "log_thread.h"
#include "logger_def.h"
#include "base_thread.h"

#define LOG_INIT(log_path) \
    do { \
        log_thread::init(log_path); \
        log_thread::wait_for_init(); \
    } while (0)

// Legacy compatibility - redirects to type-safe versions
#define LOG_WRITE(t, ...) log_thread::log_write_safe(t, __VA_ARGS__)
#define FILE_WRITE(f, ...) log_thread::log_write_safe(f, __VA_ARGS__)

// Legacy printf-style macros - deprecated, use LOG_XXX_SAFE instead
#define LOG_WARNING(...) LOG_WARNING_SAFE(__VA_ARGS__)
#define LOG_FATAL(...) LOG_FATAL_SAFE(__VA_ARGS__)
#define LOG_NOTICE(...) LOG_NOTICE_SAFE(__VA_ARGS__)
#define LOG_TRACE(...) LOG_TRACE_SAFE(__VA_ARGS__)
#define LOG_DEBUG(...) LOG_DEBUG_SAFE(__VA_ARGS__)

#define LOGWARNING log_stream(LOGTYPEWARNING, __LINE__, __func__, __FILE__)
#define LOGFATAL log_stream(LOGTYPEFATAL, __LINE__, __func__, __FILE__) 
#define LOGNOTICE log_stream(LOGTYPENOTICE, __LINE__, __func__, __FILE__)
#define LOGTRACE log_stream(LOGTYPETRACE, __LINE__, __func__, __FILE__)
#define LOGDEBUG log_stream(LOGTYPEDEBUG, __LINE__, __func__, __FILE__)

// Type-safe variadic template macros (C++11)
#define LOG_WRITE_SAFE(type, ...) log_thread::log_write_safe(type, __VA_ARGS__)
#define FILE_WRITE_SAFE(filename, ...) log_thread::log_write_safe(filename, __VA_ARGS__)

// Convenient type-safe logging macros
#define LOG_WARNING_SAFE(...) LOG_WRITE_SAFE(LOGTYPEWARNING, "WARNING:[", rx_get_thread_id(), "]:[", __LINE__, ":", __func__, ":", __FILE__, "] ", __VA_ARGS__)
#define LOG_FATAL_SAFE(...) LOG_WRITE_SAFE(LOGTYPEFATAL, "FATAL:[", rx_get_thread_id(), "]:[", __LINE__, ":", __func__, ":", __FILE__, "] ", __VA_ARGS__)
#define LOG_NOTICE_SAFE(...) LOG_WRITE_SAFE(LOGTYPENOTICE, "NOTICE:[", rx_get_thread_id(), "]:[", __LINE__, ":", __func__, ":", __FILE__, "] ", __VA_ARGS__)
#define LOG_TRACE_SAFE(...) LOG_WRITE_SAFE(LOGTYPETRACE, "TRACE:[", rx_get_thread_id(), "]:[", __LINE__, ":", __func__, ":", __FILE__, "] ", __VA_ARGS__)
#define LOG_DEBUG_SAFE(...) LOG_WRITE_SAFE(LOGTYPEDEBUG, "DEBUG:[", rx_get_thread_id(), "]:[", __LINE__, ":", __func__, ":", __FILE__, "] ", __VA_ARGS__)

#endif 
