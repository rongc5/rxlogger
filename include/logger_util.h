#ifndef __LOGGER_UTIL_H__
#define __LOGGER_UTIL_H__

#include "logger_def.h"

// ���ߺ�������
char* im_chomp(char* str, char c = '\n');
char* trim(const char* str);
void get_proc_name(char* name, size_t size);
void get_timestr(char* buf, size_t size, const char* format);
void get_timestr_millSecond(char* buf, size_t size, const char* format);
string strError(int error_code);

// �ַ����ָ��
void SplitString(const string& str, const string& delimiter, vector<string>* result, int mode = 0);

// ���ý������
template<typename K, typename V>
bool has_key(const map<K, V>& m, const K& key) {
    return m.find(key) != m.end();
}

// �쳣�����
#define THROW_COMMON_EXCEPT(msg) \
    do { \
        stringstream ss; \
        ss << msg; \
        throw runtime_error(ss.str()); \
    } while(0)

#endif 