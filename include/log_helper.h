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

// Legacy compatibility - generic wrappers
#define LOG_WRITE(t, ...) log_thread::log_write_safe(t, __VA_ARGS__)
#define FILE_WRITE(f, ...) log_thread::log_write_safe(f, __VA_ARGS__)

// printf-style logging (compat with historical usage)
#define LOG_WARNINGF(fmt, ...) LOG_WRITE_SAFE(LOGTYPEWARNING, rx_format(fmt, ##__VA_ARGS__))
#define LOG_FATALF(fmt, ...)   LOG_WRITE_SAFE(LOGTYPEFATAL,   rx_format(fmt, ##__VA_ARGS__))
#define LOG_NOTICEF(fmt, ...)  LOG_WRITE_SAFE(LOGTYPENOTICE,  rx_format(fmt, ##__VA_ARGS__))
#define LOG_TRACEF(fmt, ...)   LOG_WRITE_SAFE(LOGTYPETRACE,   rx_format(fmt, ##__VA_ARGS__))
#define LOG_DEBUGF(fmt, ...)   LOG_WRITE_SAFE(LOGTYPEDEBUG,   rx_format(fmt, ##__VA_ARGS__))

// printf-style file logging
#define FILE_WRITEF(filename, fmt, ...) FILE_WRITE_SAFE(filename, rx_format(fmt, ##__VA_ARGS__))

// Map bare LOG_* to prefixed printf-style (include level/thread/pos prefix)
#define LOG_WARNING(...) LOG_WARNING_SAFE(rx_format(__VA_ARGS__))
#define LOG_FATAL(...)   LOG_FATAL_SAFE(rx_format(__VA_ARGS__))
#define LOG_NOTICE(...)  LOG_NOTICE_SAFE(rx_format(__VA_ARGS__))
#define LOG_TRACE(...)   LOG_TRACE_SAFE(rx_format(__VA_ARGS__))
#define LOG_DEBUG(...)   LOG_DEBUG_SAFE(rx_format(__VA_ARGS__))
// Map bare FILE_WRITE to printf-style for backward compatibility
#undef FILE_WRITE
#define FILE_WRITE(filename, ...) FILE_WRITEF(filename, __VA_ARGS__)

#define LOGWARNING log_stream(LOGTYPEWARNING, __LINE__, __func__, __FILE__)
#define LOGFATAL log_stream(LOGTYPEFATAL, __LINE__, __func__, __FILE__) 
#define LOGNOTICE log_stream(LOGTYPENOTICE, __LINE__, __func__, __FILE__)
#define LOGTRACE log_stream(LOGTYPETRACE, __LINE__, __func__, __FILE__)
#define LOGDEBUG log_stream(LOGTYPEDEBUG, __LINE__, __func__, __FILE__)

// Type-safe variadic template macros (C++11)
#define LOG_WRITE_SAFE(type, ...) log_thread::log_write_safe(type, __VA_ARGS__)
#define FILE_WRITE_SAFE(filename, ...) log_thread::log_write_safe(filename, __VA_ARGS__)

// Convenient type-safe logging macros
#define LOG_WARNING_SAFE(...) LOG_WRITE_SAFE(LOGTYPEWARNING, "WARNING:[", rx_get_thread_id(), "]:[", __LINE__, ":", __func__, ":", rx_basename(__FILE__), "] ", __VA_ARGS__)
#define LOG_FATAL_SAFE(...)   LOG_WRITE_SAFE(LOGTYPEFATAL,   "FATAL:[",   rx_get_thread_id(), "]:[", __LINE__, ":", __func__, ":", rx_basename(__FILE__), "] ", __VA_ARGS__)
#define LOG_NOTICE_SAFE(...)  LOG_WRITE_SAFE(LOGTYPENOTICE,  "NOTICE:[",  rx_get_thread_id(), "]:[", __LINE__, ":", __func__, ":", rx_basename(__FILE__), "] ", __VA_ARGS__)
#define LOG_TRACE_SAFE(...)   LOG_WRITE_SAFE(LOGTYPETRACE,   "TRACE:[",   rx_get_thread_id(), "]:[", __LINE__, ":", __func__, ":", rx_basename(__FILE__), "] ", __VA_ARGS__)
#define LOG_DEBUG_SAFE(...)   LOG_WRITE_SAFE(LOGTYPEDEBUG,   "DEBUG:[",   rx_get_thread_id(), "]:[", __LINE__, ":", __func__, ":", rx_basename(__FILE__), "] ", __VA_ARGS__)

#endif 
