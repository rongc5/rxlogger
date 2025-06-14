#include "../include/logger_util.h"

char* im_chomp(char* str, char c) {
    if (!str) return NULL;
    
    char* end = str + strlen(str) - 1;
    while (end >= str && *end == c) {
        *end = '\0';
        end--;
    }
    
    // ����ͷ���ַ�
    while (*str == c) {
        str++;
    }
    
    return str;
}

char* trim(const char* str) {
    if (!str) return NULL;
    
    // ������ͷ�Ŀհ��ַ�
    while (*str && (*str == ' ' || *str == '\t' || *str == '\n' || *str == '\r')) {
        str++;
    }
    
    if (*str == '\0') {
        static char empty[] = "";
        return empty;
    }
    
    // �ҵ���β
    const char* end = str + strlen(str) - 1;
    while (end > str && (*end == ' ' || *end == '\t' || *end == '\n' || *end == '\r')) {
        end--;
    }
    
    // �����ڴ沢�����ַ���
    size_t len = end - str + 1;
    char* result = (char*)malloc(len + 1);
    if (result) {
        strncpy(result, str, len);
        result[len] = '\0';
    }
    
    return result;
}

void get_proc_name(char* name, size_t size) {
    if (!name || size == 0) return;
    
    // Linuxʵ�֣���/proc/self/comm��ȡ������
    FILE* file = fopen("/proc/self/comm", "r");
    if (file) {
        if (fgets(name, size, file)) {
            // ȥ�����з�
            char* newline = strchr(name, '\n');
            if (newline) *newline = '\0';
        } else {
            strncpy(name, "unknown", size - 1);
            name[size - 1] = '\0';
        }
        fclose(file);
    } else {
        // ���÷�����ʹ��"logger"��ΪĬ������
        strncpy(name, "logger", size - 1);
        name[size - 1] = '\0';
    }
}

void get_timestr(char* buf, size_t size, const char* format) {
    if (!buf || size == 0 || !format) return;
    
    time_t now = time(NULL);
    struct tm* timeinfo = localtime(&now);
    strftime(buf, size, format, timeinfo);
}

void get_timestr_millSecond(char* buf, size_t size, const char* format) {
    if (!buf || size == 0 || !format) return;
    
    struct timeval tv;
    gettimeofday(&tv, NULL);
    
    struct tm* timeinfo = localtime(&tv.tv_sec);
    size_t len = strftime(buf, size, format, timeinfo);
    
    // ��Ӻ���
    if (len < size - 4) {
        snprintf(buf + len, size - len, ".%03d", (int)(tv.tv_usec / 1000));
    }
}

string strError(int error_code) {
    return string(strerror(error_code));
}

void SplitString(const string& str, const string& delimiter, vector<string>* result, int mode) {
    if (!result) return;
    
    result->clear();
    
    if (str.empty() || delimiter.empty()) {
        result->push_back(str);
        return;
    }
    
    size_t start = 0;
    size_t found = 0;
    
    while ((found = str.find(delimiter, start)) != string::npos) {
        string token = str.substr(start, found - start);
        
        if (mode & SPLIT_MODE_TRIM) {
            char* trimmed = trim(token.c_str());
            if (trimmed) {
                result->push_back(string(trimmed));
                free(trimmed);
            }
        } else {
            result->push_back(token);
        }
        
        start = found + delimiter.length();
        
        if (mode & SPLIT_MODE_ONE) {
            // ֻ�ָ�һ��
            string remaining = str.substr(start);
            if (mode & SPLIT_MODE_TRIM) {
                char* trimmed = trim(remaining.c_str());
                if (trimmed) {
                    result->push_back(string(trimmed));
                    free(trimmed);
                }
            } else {
                result->push_back(remaining);
            }
            break;
        }
    }
    
    if (start < str.length()) {
        string token = str.substr(start);
        if (mode & SPLIT_MODE_TRIM) {
            char* trimmed = trim(token.c_str());
            if (trimmed) {
                result->push_back(string(trimmed));
                free(trimmed);
            }
        } else {
            result->push_back(token);
        }
    }
} 