#ifndef __LOG_HELPER_H_
#define __LOG_HELPER_H_

#include "log_thread.h"
#include "logger_def.h"
#include "base_thread.h"

#ifndef RXLOGGER_NO_MACROS
#define LOG_INIT(log_path) \
    do { \
        rxlogger::log_thread::init(log_path); \
        rxlogger::log_thread::wait_for_init(); \
    } while (0)

// Legacy compatibility - generic wrappers
#define LOG_WRITE(t, ...) rxlogger::log_thread::log_write_safe(t, __VA_ARGS__)
#define FILE_WRITE(f, ...) rxlogger::log_thread::log_write_safe(f, __VA_ARGS__)

// printf-style logging (compat with historical usage)
#define LOG_WARNINGF(fmt, ...) LOG_WRITE_SAFE(rxlogger::LOGTYPEWARNING, rxlogger::rx_format(fmt, ##__VA_ARGS__))
#define LOG_FATALF(fmt, ...)   LOG_WRITE_SAFE(rxlogger::LOGTYPEFATAL,   rxlogger::rx_format(fmt, ##__VA_ARGS__))
#define LOG_NOTICEF(fmt, ...)  LOG_WRITE_SAFE(rxlogger::LOGTYPENOTICE,  rxlogger::rx_format(fmt, ##__VA_ARGS__))
#define LOG_TRACEF(fmt, ...)   LOG_WRITE_SAFE(rxlogger::LOGTYPETRACE,   rxlogger::rx_format(fmt, ##__VA_ARGS__))
#define LOG_DEBUGF(fmt, ...)   LOG_WRITE_SAFE(rxlogger::LOGTYPEDEBUG,   rxlogger::rx_format(fmt, ##__VA_ARGS__))

// printf-style file logging
#define FILE_WRITEF(filename, fmt, ...) FILE_WRITE_SAFE(filename, rxlogger::rx_format(fmt, ##__VA_ARGS__))

// Map bare LOG_* to prefixed printf-style (include level/thread/pos prefix)
#define LOG_WARNING(...) LOG_WARNING_SAFE(rxlogger::rx_format(__VA_ARGS__))
#define LOG_FATAL(...)   LOG_FATAL_SAFE(rxlogger::rx_format(__VA_ARGS__))
#define LOG_NOTICE(...)  LOG_NOTICE_SAFE(rxlogger::rx_format(__VA_ARGS__))
#define LOG_TRACE(...)   LOG_TRACE_SAFE(rxlogger::rx_format(__VA_ARGS__))
#define LOG_DEBUG(...)   LOG_DEBUG_SAFE(rxlogger::rx_format(__VA_ARGS__))
// Map bare FILE_WRITE to printf-style for backward compatibility
#undef FILE_WRITE
#define FILE_WRITE(filename, ...) FILE_WRITEF(filename, __VA_ARGS__)

#define LOGWARNING rxlogger::log_stream(rxlogger::LOGTYPEWARNING, __LINE__, __func__, __FILE__)
#define LOGFATAL   rxlogger::log_stream(rxlogger::LOGTYPEFATAL,   __LINE__, __func__, __FILE__) 
#define LOGNOTICE  rxlogger::log_stream(rxlogger::LOGTYPENOTICE,  __LINE__, __func__, __FILE__)
#define LOGTRACE   rxlogger::log_stream(rxlogger::LOGTYPETRACE,   __LINE__, __func__, __FILE__)
#define LOGDEBUG   rxlogger::log_stream(rxlogger::LOGTYPEDEBUG,   __LINE__, __func__, __FILE__)

// Type-safe variadic template macros (C++11)
#define LOG_WRITE_SAFE(type, ...) rxlogger::log_thread::log_write_safe(type, __VA_ARGS__)
#define FILE_WRITE_SAFE(filename, ...) rxlogger::log_thread::log_write_safe(filename, __VA_ARGS__)

// Convenient type-safe logging macros
#define LOG_WARNING_SAFE(...) LOG_WRITE_SAFE(rxlogger::LOGTYPEWARNING, "WARNING:[", rxlogger::rx_get_thread_id(), "]:[", __LINE__, ":", __func__, ":", rxlogger::rx_basename(__FILE__), "] ", __VA_ARGS__)
#define LOG_FATAL_SAFE(...)   LOG_WRITE_SAFE(rxlogger::LOGTYPEFATAL,   "FATAL:[",   rxlogger::rx_get_thread_id(), "]:[", __LINE__, ":", __func__, ":", rxlogger::rx_basename(__FILE__), "] ", __VA_ARGS__)
#define LOG_NOTICE_SAFE(...)  LOG_WRITE_SAFE(rxlogger::LOGTYPENOTICE,  "NOTICE:[",  rxlogger::rx_get_thread_id(), "]:[", __LINE__, ":", __func__, ":", rxlogger::rx_basename(__FILE__), "] ", __VA_ARGS__)
#define LOG_TRACE_SAFE(...)   LOG_WRITE_SAFE(rxlogger::LOGTYPETRACE,   "TRACE:[",   rxlogger::rx_get_thread_id(), "]:[", __LINE__, ":", __func__, ":", rxlogger::rx_basename(__FILE__), "] ", __VA_ARGS__)
#define LOG_DEBUG_SAFE(...)   LOG_WRITE_SAFE(rxlogger::LOGTYPEDEBUG,   "DEBUG:[",   rxlogger::rx_get_thread_id(), "]:[", __LINE__, ":", __func__, ":", rxlogger::rx_basename(__FILE__), "] ", __VA_ARGS__)

#endif /* RXLOGGER_NO_MACROS */

#endif 
