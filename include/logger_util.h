#ifndef __LOGGER_UTIL_H__
#define __LOGGER_UTIL_H__

#include "logger_def.h"

// 工具函数声明
char* im_chomp(char* str, char c = '\n');
char* trim(const char* str);
void get_proc_name(char* name, size_t size);
void get_timestr(char* buf, size_t size, const char* format);
void get_timestr_millSecond(char* buf, size_t size, const char* format);
string strError(int error_code);

// 字符串分割函数
void SplitString(const string& str, const string& delimiter, vector<string>* result, int mode = 0);

// 配置解析相关
template<typename K, typename V>
bool has_key(const map<K, V>& m, const K& key) {
    return m.find(key) != m.end();
}

// 异常处理宏
#define THROW_COMMON_EXCEPT(msg) \
    do { \
        stringstream ss; \
        ss << msg; \
        throw runtime_error(ss.str()); \
    } while(0)

#endif 