#ifndef __LOG_BASE_H_
#define __LOG_BASE_H_

#include "logger_def.h"
#include "logger_util.h"


enum LogType {
    LOGTYPEFATAL = 1,
    LOGTYPEWARNING = 2,
    LOGTYPENOTICE = 4,
    LOGTYPETRACE = 8,
    LOGTYPEDEBUG = 16, 
    LOGTYPESIZE
};

enum LogModel {
    LOGLOCAL = 1,
    LOGTHREAD = 2,
    LOGMODELSIZE
};

// Simplified reload interface base class
class reload_inf {
public:
    virtual ~reload_inf() {}
    virtual int load() = 0;
    virtual int reload() = 0;
    virtual bool need_reload() = 0;
    virtual int dump() = 0;
    virtual int destroy() = 0;
};

// Reload manager template
template<typename T>
class reload_mgr {
private:
    std::unique_ptr<T> _current;
    std::unique_ptr<T> _backup;
    std::mutex _mutex;

public:
    reload_mgr(std::unique_ptr<T> current, std::unique_ptr<T> backup) 
        : _current(std::move(current)), _backup(std::move(backup)) {}
    
    T* current() {
        std::lock_guard<std::mutex> lock(_mutex);
        return _current.get();
    }
    
    int reload() {
        std::lock_guard<std::mutex> lock(_mutex);
        if (_backup && _backup->need_reload()) {
            int ret = _backup->reload();
            if (ret == 0) {
                std::swap(_current, _backup);
            }
            return ret;
        }
        return 0;
    }
};

class log_conf : public reload_inf {
public:
    log_conf(const char* sk_conf);
    virtual int load();
    virtual int reload();
    virtual bool need_reload();
    virtual int dump();
    virtual int destroy();

private:
    void do_parse();
    void get_file_name(LogType type);

public:
    uint32_t file_max_size;
    std::string log_path;
    std::string prefix_file_name;
    LogType type;
    LogModel model;
    std::string _dumppath;
    std::string _log_name[LOGTYPESIZE];

private:
    std::map<std::string, std::string> _cfg;
    std::string _log_conf_filename;
    time_t _last_load;
};

#endif 