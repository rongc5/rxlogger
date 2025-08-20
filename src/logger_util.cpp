#include "../include/logger_util.h"

char* im_chomp(char* str, char c) {
    if (!str) return NULL;
    
    char* end = str + strlen(str) - 1;
    while (end >= str && *end == c) {
        *end = '\0';
        end--;
    }
    
    // Remove leading characters
    while (*str == c) {
        str++;
    }
    
    return str;
}

std::string trim(const std::string& str) {
    if (str.empty()) {
        return str;
    }
    
    // Find first non-whitespace character
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) {
        return std::string(); // All whitespace
    }
    
    // Find last non-whitespace character
    size_t end = str.find_last_not_of(" \t\n\r");
    
    return str.substr(start, end - start + 1);
}

void get_proc_name(char* name, size_t size) {
    if (!name || size == 0) return;
    
    // Linux implementation: get process name from /proc/self/comm
    FILE* file = fopen("/proc/self/comm", "r");
    if (file) {
        if (fgets(name, size, file)) {
            // Remove trailing newline
            char* newline = strchr(name, '\n');
            if (newline) *newline = '\0';
        } else {
            strncpy(name, "unknown", size - 1);
            name[size - 1] = '\0';
        }
        fclose(file);
    } else {
        // Fallback: use "logger" as default name
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
    
    // Append milliseconds
    if (len < size - 4) {
        snprintf(buf + len, size - len, ".%03d", (int)(tv.tv_usec / 1000));
    }
}

std::string strError(int error_code) {
    return std::string(strerror(error_code));
}

uint64_t get_thread_id() {
    auto id = std::this_thread::get_id();
    std::stringstream ss;
    ss << id;
    uint64_t thread_id = 0;
    try {
        thread_id = std::stoull(ss.str());
    } catch (...) {
        // Fallback: use hash of thread id
        thread_id = std::hash<std::thread::id>{}(id);
    }
    return thread_id;
}

void SplitString(const std::string& str, const std::string& delimiter, std::vector<std::string>* result, int mode) {
    if (!result) return;
    
    result->clear();
    
    if (str.empty() || delimiter.empty()) {
        result->push_back(str);
        return;
    }
    
    size_t start = 0;
    size_t found = 0;
    
    while ((found = str.find(delimiter, start)) != std::string::npos) {
        std::string token = str.substr(start, found - start);
        
        if (mode & SPLIT_MODE_TRIM) {
            token = trim(token);
        }
        result->push_back(token);
        
        start = found + delimiter.length();
        
        if (mode & SPLIT_MODE_ONE) {
            // Only split once
            std::string remaining = str.substr(start);
            if (mode & SPLIT_MODE_TRIM) {
                remaining = trim(remaining);
            }
            result->push_back(remaining);
            break;
        }
    }
    
    if (start < str.length()) {
        std::string token = str.substr(start);
        if (mode & SPLIT_MODE_TRIM) {
            token = trim(token);
        }
        result->push_back(token);
    }
} 