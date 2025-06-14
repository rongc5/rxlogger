#ifndef __LOG_BASE_H_
#define __LOG_BASE_H_

#include "logger_def.h"
#include "logger_util.h"

using namespace std;

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

// 简化的重载接口基类
class reload_inf {
public:
    virtual ~reload_inf() {}
    virtual int load() = 0;
    virtual int reload() = 0;
    virtual bool need_reload() = 0;
    virtual int dump() = 0;
    virtual int destroy() = 0;
};

// 重载管理器模板
template<typename T>
class reload_mgr {
private:
    T* _current;
    T* _backup;
    std::mutex _mutex;

public:
    reload_mgr(T* current, T* backup) : _current(current), _backup(backup) {}
    
    ~reload_mgr() {
        delete _current;
        delete _backup;
    }
    
    T* current() {
        std::lock_guard<std::mutex> lock(_mutex);
        return _current;
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
    string log_path;
    string prefix_file_name;
    LogType type;
    LogModel model;
    string _dumppath;
    string _log_name[LOGTYPESIZE];

private:
    map<string, string> _cfg;
    string _log_conf_filename;
    time_t _last_load;
};

#endif 