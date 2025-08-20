#ifndef __LOGGER_UTIL_H__
#define __LOGGER_UTIL_H__

#include "logger_def.h"

// Utility functions
char* im_chomp(char* str, char c = '\n');
std::string trim(const std::string& str);
void get_proc_name(char* name, size_t size);
void get_timestr(char* buf, size_t size, const char* format);
void get_timestr_millSecond(char* buf, size_t size, const char* format);
std::string strError(int error_code);
uint64_t get_thread_id();

// String splitting
void SplitString(const std::string& str, const std::string& delimiter, std::vector<std::string>* result, int mode = 0);

// Collection utilities
template<typename K, typename V>
bool has_key(const std::map<K, V>& m, const K& key) {
    return m.find(key) != m.end();
}

// Exception utilities
#define THROW_COMMON_EXCEPT(msg) \
    do { \
        std::stringstream ss; \
        ss << msg; \
        throw std::runtime_error(ss.str()); \
    } while(0)

#endif 