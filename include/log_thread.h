#ifndef __LOG_THREAD_H__
#define __LOG_THREAD_H__

#include "base_thread.h"
#include "log_base.h"

class log_thread;

// Helper for C++11 variadic template recursion
namespace detail {
    inline void write_to_stream(std::stringstream&) {
        // Base case: do nothing
    }
    
    template<typename T, typename... Args>
    void write_to_stream(std::stringstream& ss, const T& first, const Args&... rest) {
        ss << first;
        write_to_stream(ss, rest...);
    }
}

class log_msg {
public:
    log_msg() : _type(LOGTYPEDEBUG), _logid(0) {}

    LogType _type;
    std::string _fname;
    std::unique_ptr<std::vector<char>> _buf;
    uint64_t _logid;
};

class log_thread : public rx_base_thread {
public:
    log_thread();
    virtual ~log_thread();

    static void init(const char* path);
    static void wait_for_init();
    
    // Type-safe variadic template versions
    template<typename... Args>
    static void log_write_safe(LogType type, const Args&... args);
    template<typename... Args>
    static void log_write_safe(const char* filename, const Args&... args);
    
    static uint64_t get_logid();

    void handle_msg(std::shared_ptr<log_msg>& p_msg);
    virtual void* run();
    void obj_process();
    void put_msg(std::shared_ptr<log_msg>& p_msg);
    bool check_type(LogType type);
    
private:
    void log_thread_init(const char* path);
    void log_conf_init();
    void check_to_renmae(const char* filename, int max_size);
    int RECV(int fd, void* buf, size_t len);
    size_t process_recv_buf(const char* buf, const size_t len);

public:
    std::string _proc_name;
    
private:
    int _epoll_fd; 
    std::mutex _mutex;
    std::deque<std::shared_ptr<log_msg> > _queue[2];
    int _channelid;
    struct epoll_event* _epoll_events;
    uint32_t _epoll_size;
    std::string _recv_buf;
    volatile int _current;
    std::unique_ptr<reload_mgr<log_conf>> _rlog_conf; 
    static uint64_t log_global_id;
    static std::mutex _init_mutex;
    static std::condition_variable _init_cv;
    static bool _init_completed;
};

class log_stream {
public:
    log_stream(LogType type, int line, std::string func, std::string file) {
        switch (type) {
            case LOGTYPEDEBUG:
                ss << "DEBUG:[" << std::this_thread::get_id() << "]:[" << line <<":" << func <<":"<< file << "]";
                break;
            case LOGTYPETRACE:
                ss << "TRACE:[" << std::this_thread::get_id() << "]:[" << line <<":" << func <<":"<< file << "]";
                break;
            case LOGTYPENOTICE:
                ss << "NOTICE:[" << std::this_thread::get_id() << "]:[" << line <<":" << func <<":"<< file << "]";
                break;
            case LOGTYPEFATAL:
                ss << "FATAL:[" << std::this_thread::get_id() << "]:[" << line <<":" << func <<":"<< file << "]";
                break;
            case LOGTYPEWARNING:
                ss << "WARNING:[" << std::this_thread::get_id() << "]:[" << line <<":" << func <<":"<< file << "]";
                break;
            default:
                break;
        }
        _type = type;
    }

    ~log_stream() {
        log_thread::log_write_safe(_type, ss.str());
    }
    
    template <typename T>
    log_stream& operator<<(const T& data) {
        ss << data;
        return *this;
    }
    
    // Type-safe variadic log method
    template<typename... Args>
    void log_safe(const Args&... args) {
        detail::write_to_stream(ss, args...);
    }

    std::string str() {   
        return ss.str();
    }  
    
private:
    std::stringstream ss;
    LogType _type;
};

// Template method implementations
template<typename... Args>
void log_thread::log_write_safe(LogType type, const Args&... args) {
    log_thread* thread = rx_base_singleton<log_thread>::get_instance();
    if (!thread || !thread->check_type(type)) {
        return;
    }
    
    std::stringstream ss;
    char log_common_tmp[RX_SIZE_LEN_64];
    rx_get_timestr_millSecond(log_common_tmp, sizeof(log_common_tmp), LOG_DATE_FORMAT);
    
    ss << "[" << thread->_proc_name << "]:[" << log_common_tmp << "] ";
    
    // Use C++11 recursive template approach
    detail::write_to_stream(ss, args...);
    
    std::shared_ptr<log_msg> lmsg = std::make_shared<log_msg>();
    std::string log_content = ss.str();
    lmsg->_buf.reset(new std::vector<char>(log_content.begin(), log_content.end()));
    lmsg->_buf->push_back('\0'); // Null terminate
    lmsg->_type = type;
    thread->put_msg(lmsg);
}

template<typename... Args>
void log_thread::log_write_safe(const char* filename, const Args&... args) {
    log_thread* thread = rx_base_singleton<log_thread>::get_instance();
    if (!thread) {
        return;
    }
    
    std::stringstream ss;
    detail::write_to_stream(ss, args...);
    
    std::shared_ptr<log_msg> lmsg = std::make_shared<log_msg>();
    std::string log_content = ss.str();
    lmsg->_buf.reset(new std::vector<char>(log_content.begin(), log_content.end()));
    lmsg->_buf->push_back('\0'); // Null terminate
    lmsg->_fname.append(filename);
    thread->put_msg(lmsg);
}

#endif 
