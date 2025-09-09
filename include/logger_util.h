#ifndef __LOGGER_UTIL_H__
#define __LOGGER_UTIL_H__

#include "logger_def.h"

// Utility functions (prefixed to avoid collision)
char* rx_im_chomp(char* str, char c = '\n');
std::string rx_trim(const std::string& str);
void rx_get_proc_name(char* name, size_t size);
void rx_get_timestr(char* buf, size_t size, const char* format);
void rx_get_timestr_millSecond(char* buf, size_t size, const char* format);
std::string rx_strError(int error_code);
uint64_t rx_get_thread_id();

// String splitting
void rx_SplitString(const std::string& str, const std::string& delimiter, std::vector<std::string>* result, int mode = 0);

// Collection utilities
template<typename K, typename V>
inline bool rx_has_key(const std::map<K, V>& m, const K& key) {
    return m.find(key) != m.end();
}

// Exception utilities
#define RX_THROW_COMMON_EXCEPT(msg) \
    do { \
        std::stringstream ss; \
        ss << msg; \
        throw std::runtime_error(ss.str()); \
    } while(0)

#endif 
