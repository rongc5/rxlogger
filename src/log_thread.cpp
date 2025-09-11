#include "../include/log_thread.h"
#include "../include/base_thread.h"

namespace rxlogger {

// File handle cache implementation
file_handle_cache::file_handle_cache() {}

file_handle_cache::~file_handle_cache() {
    close_all();
}

FILE* file_handle_cache::get_file_handle_stdio(const std::string& path) {
    std::lock_guard<std::mutex> lock(_cache_mutex);
    
    auto it = _file_cache.find(path);
    if (it != _file_cache.end() && it->second.fp) {
        // update LRU
        auto it2 = _lru_pos.find(path);
        if (it2 != _lru_pos.end()) {
            _lru.erase(it2->second);
            _lru.push_front(path);
            _lru_pos[path] = _lru.begin();
        }
        return it->second.fp;
    }
    
    // Create new file handle
    if (_file_cache.size() >= _capacity && !_lru.empty()) {
        const std::string& victim = _lru.back();
        auto vit = _file_cache.find(victim);
        if (vit != _file_cache.end()) {
            close_file_info(vit->second);
            _file_cache.erase(vit);
        }
        _lru.pop_back();
        _lru_pos.erase(victim);
    }
    FILE* fp = fopen(path.c_str(), "a");
    if (fp) {
        file_info& info = _file_cache[path];
        info.fp = fp;
        info.fd = -1;
        info.path = path;
        _lru.push_front(path);
        _lru_pos[path] = _lru.begin();
    }
    
    return fp;
}

void file_handle_cache::close_file_info(file_info& info) {
    if (info.fp) { fclose(info.fp); info.fp = nullptr; }
    if (info.fd >= 0) { ::close(info.fd); info.fd = -1; }
}

void file_handle_cache::close_all() {
    std::lock_guard<std::mutex> lock(_cache_mutex);
    for (auto& pair : _file_cache) {
        close_file_info(pair.second);
    }
    _file_cache.clear();
    _lru.clear();
    _lru_pos.clear();
}

void file_handle_cache::close_file(const std::string& path) {
    std::lock_guard<std::mutex> lock(_cache_mutex);
    auto it = _file_cache.find(path);
    if (it != _file_cache.end()) {
        close_file_info(it->second);
        _file_cache.erase(it);
        auto it2 = _lru_pos.find(path);
        if (it2 != _lru_pos.end()) {
            _lru.erase(it2->second);
            _lru_pos.erase(it2);
        }
    }
}

// New methods for fd mode + LRU management
int file_handle_cache::get_file_handle_fd(const std::string& path) {
    std::lock_guard<std::mutex> lock(_cache_mutex);
    auto it = _file_cache.find(path);
    if (it != _file_cache.end() && it->second.fd >= 0) {
        auto it2 = _lru_pos.find(path);
        if (it2 != _lru_pos.end()) {
            _lru.erase(it2->second);
            _lru.push_front(path);
            _lru_pos[path] = _lru.begin();
        }
        return it->second.fd;
    }
    if (_file_cache.size() >= _capacity && !_lru.empty()) {
        const std::string& victim = _lru.back();
        auto vit = _file_cache.find(victim);
        if (vit != _file_cache.end()) {
            close_file_info(vit->second);
            _file_cache.erase(vit);
        }
        _lru.pop_back();
        _lru_pos.erase(victim);
    }
    int fd = ::open(path.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd >= 0) {
        file_info& info = _file_cache[path];
        info.fd = fd;
        info.fp = nullptr;
        info.path = path;
        _lru.push_front(path);
        _lru_pos[path] = _lru.begin();
    }
    return fd;
}

void file_handle_cache::set_capacity(size_t cap) {
    std::lock_guard<std::mutex> lock(_cache_mutex);
    _capacity = cap ? cap : 1;
    while (_file_cache.size() > _capacity) {
        const std::string& victim = _lru.back();
        auto vit = _file_cache.find(victim);
        if (vit != _file_cache.end()) {
            close_file_info(vit->second);
            _file_cache.erase(vit);
        }
        _lru.pop_back();
        _lru_pos.erase(victim);
    }
}

uint64_t log_thread::log_global_id = 0;
std::mutex log_thread::_init_mutex;
std::condition_variable log_thread::_init_cv;
bool log_thread::_init_completed = false;

log_thread::log_thread() {
    _epoll_fd = -1;
    _channelid = -1;
    _epoll_events = NULL;
    _epoll_size = 0;
    _current = 0;
    _rlog_conf = NULL;
}

log_thread::~log_thread() {
    if (_epoll_events != NULL) {
        delete [] _epoll_events;
    }
    
    if (_epoll_fd != -1) {
        close(_epoll_fd);
    }
    
    if (_channelid != -1) {
        close(_channelid);
    }
    
    if (_rlog_conf) {
        delete _rlog_conf;
        _rlog_conf = NULL;
    }

    _queue[_current].clear();
    _queue[1- _current].clear();
}

void log_thread::init(const char* path) {
    log_thread* thread = rx_base_singleton<log_thread>::get_instance();
    thread->log_thread_init(path);
    thread->start();
}

void log_thread::wait_for_init() {
    std::unique_lock<std::mutex> lock(_init_mutex);
    _init_cv.wait(lock, []{ return _init_completed; });
}


bool log_thread::check_type(LogType type) {
    if (!_rlog_conf) {
        return false;
    }
    log_conf* conf = _rlog_conf->current();
    if (conf && (conf->type & type) != 0) {
        return true;
    }
    return false;
}


void log_thread::put_msg(std::shared_ptr<log_msg>& p_msg) {
    log_conf* conf = _rlog_conf->current();

    std::lock_guard<std::mutex> lck(_mutex);
    p_msg->_logid = get_logid();

    if (conf->model == LOGLOCAL) {
        handle_msg(p_msg); 
    } else {
        int idle = 1 - _current;
        _queue[idle].push_back(p_msg);
    }

    send(_channelid, CHANNEL_MSG_TAG, sizeof(CHANNEL_MSG_TAG), MSG_DONTWAIT);
}

void log_thread::handle_msg(std::shared_ptr<log_msg>& p_msg) {
    FILE* fp = nullptr;
    int fd = -1;
    
    log_conf* conf = _rlog_conf->current();

    if (p_msg->_fname.empty() && conf) {
        const std::string& path = conf->_log_name[p_msg->_type];
        // reduce stat cost
        if (should_check_rename(path, conf)) {
            check_to_renmae(path.c_str(), conf->file_max_size);
        }

        if (conf->direct_write) {
            fd = _file_cache.get_file_handle_fd(path);
            if (fd < 0) return;
            const char* payload = p_msg->_buf->data();
            char head[64];
            int headlen = snprintf(head, sizeof(head), "[log_id:%" PRIu64 "]", p_msg->_logid);
            struct iovec iov[3];
            iov[0].iov_base = head; iov[0].iov_len = (size_t)headlen;
            iov[1].iov_base = (void*)payload; iov[1].iov_len = strlen(payload);
            char nl = '\n';
            iov[2].iov_base = &nl; iov[2].iov_len = 1;
            ssize_t w = writev(fd, iov, 3);
            if (w < 0) {
                _file_cache.close_file(path);
                fd = _file_cache.get_file_handle_fd(path);
                if (fd >= 0) { ssize_t w2 = writev(fd, iov, 3); (void)w2; }
            }
        } else {
            fp = _file_cache.get_file_handle_stdio(path);
            if (!fp) return;
            if (fprintf(fp, "[log_id:%" PRIu64 "]%s\n", p_msg->_logid, p_msg->_buf->data()) < 0) {
                _file_cache.close_file(path);
                fp = _file_cache.get_file_handle_stdio(path);
                if (fp) { fprintf(fp, "[log_id:%" PRIu64 "]%s\n", p_msg->_logid, p_msg->_buf->data()); }
            }
        }
    } else {
        if (conf && conf->direct_write) {
            fd = _file_cache.get_file_handle_fd(p_msg->_fname);
            if (fd < 0) return;
            const char* payload = p_msg->_buf->data();
            struct iovec iov[2];
            iov[0].iov_base = (void*)payload; iov[0].iov_len = strlen(payload);
            char nl = '\n';
            iov[1].iov_base = &nl; iov[1].iov_len = 1;
            ssize_t w = writev(fd, iov, 2);
            if (w < 0) {
                _file_cache.close_file(p_msg->_fname);
                fd = _file_cache.get_file_handle_fd(p_msg->_fname);
                if (fd >= 0) { ssize_t w2 = writev(fd, iov, 2); (void)w2; }
            }
        } else {
            fp = _file_cache.get_file_handle_stdio(p_msg->_fname);
            if (!fp) return;
            if (fprintf(fp, "%s\n", p_msg->_buf->data()) < 0) {
                _file_cache.close_file(p_msg->_fname);
                fp = _file_cache.get_file_handle_stdio(p_msg->_fname);
                if (fp) { fprintf(fp, "%s\n", p_msg->_buf->data()); }
            }
        }
    }
}

void log_thread::check_to_renmae(const char* filename, int max_size) {
    if (!filename) {
        return;
    }

    char tmp[RX_SIZE_LEN_64];
    char path[RX_SIZE_LEN_128];
    struct stat statBuf;
    
    if (stat(filename, &statBuf) != 0) {
        return;  // File does not exist
    }

    if (max_size && statBuf.st_size >= max_size) {
        // Close cached file handle before renaming
        _file_cache.close_file(filename);
        
        rx_get_timestr(tmp, sizeof(tmp), "%Y%m%d%H%M%S");
        snprintf(path, sizeof(path), "%s.%s", filename, tmp);
        rename(filename, path);
    }
}

static inline uint64_t now_ms_local() {
    struct timeval tv; gettimeofday(&tv, nullptr);
    return (uint64_t)tv.tv_sec * 1000ULL + (tv.tv_usec / 1000ULL);
}

bool log_thread::should_check_rename(const std::string& path, const log_conf* conf) {
    if (!conf) return true;
    auto& st = _rename_states[path];
    st.counter++;
    uint64_t t = now_ms_local();
    bool by_count = (conf->rename_check_every && (st.counter % conf->rename_check_every == 0));
    bool by_time = (conf->rename_check_interval_ms && (t - st.last_check_ms >= conf->rename_check_interval_ms));
    if (st.last_check_ms == 0 || by_count || by_time) {
        st.last_check_ms = t;
        return true;
    }
    return false;
}

void log_thread::log_thread_init(const char* path) {
    if (!path) {
        return;
    }

    _rlog_conf = new reload_mgr<log_conf>(
        new log_conf(path), 
        new log_conf(path)
    );
    log_conf_init();

    _epoll_fd = epoll_create(DAFAULT_EPOLL_SIZE);
    if (_epoll_fd == -1) {       
        RX_THROW_COMMON_EXCEPT("epoll_create fail " << rx_strError(errno).c_str());
    }

    _epoll_size = RX_SIZE_LEN_16;
    _epoll_events = new epoll_event[_epoll_size];

    int fd[2];
    int ret = socketpair(AF_UNIX, SOCK_STREAM, 0, fd);
    if (ret < 0) {
        return;
    }

    _channelid = fd[1];
    _current = 0;

    struct epoll_event tmpEvent;
    memset(&tmpEvent, 0, sizeof(epoll_event));
    tmpEvent.events = EPOLLIN | EPOLLERR | EPOLLHUP;
    tmpEvent.data.fd = fd[0];
    ret = epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd[0], &tmpEvent);
    if (ret != 0) {
        RX_THROW_COMMON_EXCEPT("add to epoll fail " << rx_strError(errno).c_str());
    }

    fcntl(_channelid, F_SETFL, O_NONBLOCK);
    
    char tmp_buff[RX_SIZE_LEN_128];
    rx_get_proc_name(tmp_buff, sizeof(tmp_buff));
    _proc_name.append(tmp_buff);
}

void log_thread::log_conf_init() {
    if (!_rlog_conf) {
        return;
    }
    
    _rlog_conf->reload();

    log_conf* conf = _rlog_conf->current();
    if (conf && !conf->log_path.empty()) {
        char buf[RX_SIZE_LEN_512];
        snprintf(buf, sizeof(buf), "mkdir -p %s", conf->log_path.c_str());
        int sysrc = system(buf); (void)sysrc;
        // Apply tunables
        _file_cache.set_capacity(conf->fd_cache_capacity ? conf->fd_cache_capacity : 128);
        _file_cache.set_direct_write(conf->direct_write);
    }
}

void* log_thread::run() {
    // Notify that initialization is complete and thread is running
    {
        std::lock_guard<std::mutex> lock(_init_mutex);
        _init_completed = true;
    }
    _init_cv.notify_all();
    
    while (get_run_flag()) {
        obj_process();
    }
    return NULL;
}

int log_thread::RECV(int fd, void* buf, size_t len) {
    int ret = recv(fd, buf, len, MSG_DONTWAIT);

    if (ret == 0) {
        // Peer closed; treat as no data and continue loop
        return 0;
    } else if (ret < 0) {
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            // Non-transient error; skip this round instead of throwing
            return 0;
        }
        ret = 0;
    }

    return ret;
}

size_t log_thread::process_recv_buf(const char* buf, const size_t len) {
    std::deque<std::shared_ptr<log_msg>> processing_queue;

    {
        std::lock_guard<std::mutex> lck(_mutex);
        // If current queue is empty, flip to the other queue
        if (_queue[_current].empty()) {
            _current = 1 - _current;
        }

        // Swap the selected queue with the local processing queue (O(1))
        processing_queue.swap(_queue[_current]);
    } // Lock is released here

    // Process all messages without holding the lock
    for (auto& msg : processing_queue) {
        handle_msg(msg);
    }
    // processing_queue is automatically destroyed here, freeing memory

    return len;
}

void log_thread::obj_process() {
    int nfds = ::epoll_wait(_epoll_fd, _epoll_events, _epoll_size, DEFAULT_EPOLL_WAITE);
    if (-1 == nfds) {
        return;
    }

    char buf[RX_SIZE_LEN_2048];
    ssize_t ret = 0;
    
    for (int i = 0; i < nfds; i++) {
        if (_epoll_events[i].events & EPOLLIN) {
            ret = RECV(_epoll_events[i].data.fd, buf, sizeof(buf));
        }
    }
    
    process_recv_buf(buf, ret);

    if (_rlog_conf && _rlog_conf->need_reload()) {
        log_conf_init();
    }
}

uint64_t log_thread::get_logid() {
    if (log_global_id < LOG_ID_MIN) {
        log_global_id = LOG_ID_MIN;
    }
    return log_global_id++;
} 

} // namespace rxlogger
