#ifndef __LOG_BASE_H_
#define __LOG_BASE_H_

#include "logger_def.h"
#include "logger_util.h"

namespace rxlogger {

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

// Reload manager template - matches original myframe core implementation
template<typename T>
class reload_mgr {
private:
    T* _objects[2];
    int16_t _curr;

public:
    reload_mgr(T* T1, T* T2) {
        _objects[0] = T1;
        _objects[1] = T2;
        _curr = 0;
    }
    
    ~reload_mgr() {
        if (_objects[0]) {
            _objects[0]->destroy();
            delete _objects[0];
            _objects[0] = NULL;
        }
        if (_objects[1]) {
            _objects[1]->destroy();
            delete _objects[1];
            _objects[1] = NULL;
        }
    }
    
    int load() {
        if (_objects[_curr]->load() == 0) {
            return 0;
        }
        return -1;
    }
    
    bool need_reload() {
        return current()->need_reload();
    }
    
    int reload() {
        int16_t idle_idx = (_curr == 0) ? 1 : 0;
        if (_objects[idle_idx]->reload() == 0) {
            _curr = idle_idx;
            return 0;
        }
        return -1;
    }
    
    T* current() {
        if (_curr == 0 || _curr == 1) {
            return _objects[_curr];
        }
        return NULL;
    }
    
    T* idle() {
        int16_t idle_idx = (_curr == 0) ? 1 : 0;
        if (idle_idx == 0 || idle_idx == 1) {
            return _objects[idle_idx];
        }
        return NULL;
    }
    
    int dump() {
        T* obj = current();
        return obj ? obj->dump() : -1;
    }
    
    int destroy() {
        if (_objects[0]) _objects[0]->destroy();
        if (_objects[1]) _objects[1]->destroy();
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

} // namespace rxlogger

#endif 
