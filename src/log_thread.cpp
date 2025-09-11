#include "../include/log_thread.h"
#include "../include/base_thread.h"

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
    FILE* fp = NULL;
    
    log_conf* conf = _rlog_conf->current();

    if (p_msg->_fname.empty() && conf) {
        check_to_renmae(conf->_log_name[p_msg->_type].c_str(), conf->file_max_size);
        fp = fopen(conf->_log_name[p_msg->_type].c_str(), "a");
        if (!fp) {
            return;
        }   

        fprintf(fp, "[log_id:%" PRIu64 "]%s\n", p_msg->_logid, p_msg->_buf->data());
    } else {
        fp = fopen(p_msg->_fname.c_str(), "a");
        if (!fp) {
            return;
        }   

        fprintf(fp, "%s\n", p_msg->_buf->data());
    }

    fclose(fp);
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
        rx_get_timestr(tmp, sizeof(tmp), "%Y%m%d%H%M%S");
        snprintf(path, sizeof(path), "%s.%s", filename, tmp);
        rename(filename, path);
    }
}

void log_thread::log_thread_init(const char* path) {
    if (!path) {
        return;
    }

    _rlog_conf.reset(new reload_mgr<log_conf>(
        std::unique_ptr<log_conf>(new log_conf(path)), 
        std::unique_ptr<log_conf>(new log_conf(path))
    ));
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
        system(buf);         
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
        std::deque<std::shared_ptr<log_msg>>& queue = _queue[_current];
        
        if (queue.empty()) {
            _current = 1 - _current;
            queue = _queue[_current];
        }
        
        // Swap the queue with empty processing_queue - O(1) operation
        processing_queue.swap(queue);
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
