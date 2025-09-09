#include "../include/log_base.h"

log_conf::log_conf(const char* sk_conf) {
    _log_conf_filename.append(sk_conf);  
    _last_load = 0;
}

int log_conf::load() {
    std::ifstream file(_log_conf_filename);
    if (!file.is_open()) {
        ASSERT_WARNING(false, "open log conf failed. path[%s]", _log_conf_filename.c_str());
        return -1;
    }

    std::string line;
    std::vector<std::string> tmp_vec;
    
    while (std::getline(file, line)) {
        if (line.empty()) {
            continue;
        }

        // Remove comments and trailing whitespace
        size_t comment_pos = line.find('#');
        if (comment_pos != std::string::npos) {
            line = line.substr(0, comment_pos);
        }
        
        line = rx_trim(line);
        if (line.empty()) {
            continue;
        }

        rx_SplitString(line, "=", &tmp_vec, RX_SPLIT_MODE_ONE | RX_SPLIT_MODE_TRIM);
        if (tmp_vec.size() != 2) {
            continue;
        }

        const std::string& key = tmp_vec[0];
        const std::string& value = tmp_vec[1];
        
        if (!key.empty() && !value.empty()) {
            _cfg.insert(make_pair(key, value));
        }
    }

    file.close();
    do_parse();

    struct stat st;
    stat(_log_conf_filename.c_str(), &st);
    _last_load = st.st_mtime;

    return 0;
}

void log_conf::get_file_name(LogType type) {
    std::stringstream ss;

    switch (type) {   
        case LOGTYPEFATAL:
            ss << log_path << "/" << prefix_file_name << ".ft";
            break;
        case LOGTYPEWARNING:
            ss << log_path << "/" << prefix_file_name << ".wn";
            break;
        case LOGTYPENOTICE:
            ss << log_path << "/" << prefix_file_name << ".nt";
            break;
        case LOGTYPETRACE:
            ss << log_path << "/" << prefix_file_name << ".tc";
            break;
        case LOGTYPEDEBUG:
            ss << log_path << "/" << prefix_file_name << ".db";
            break;
        default:
            break;
    }   

    _log_name[type] = ss.str();
}

void log_conf::do_parse() {
    char buf[RX_SIZE_LEN_1024];
    rx_get_proc_name(buf, sizeof(buf));
    prefix_file_name.assign(buf);

    file_max_size = DEFAULT_LOG_MAX_SIZE;
    type = LOGTYPEDEBUG;
    log_path.assign("logs");
    model = LOGTHREAD;
    _dumppath.assign("log_conf_dump");

    if (rx_has_key<std::string, std::string>(_cfg, std::string("file_max_size"))) {    
        try {
            file_max_size = std::stoi(_cfg["file_max_size"]);
        } catch (const std::exception& e) {
            // Keep default value on conversion error
            file_max_size = DEFAULT_LOG_MAX_SIZE;
        }
    } 

    if (rx_has_key<std::string, std::string>(_cfg, std::string("log_path"))) {
        log_path = _cfg["log_path"];
    }

    if (rx_has_key<std::string, std::string>(_cfg, std::string("prefix_file_name"))) {
        prefix_file_name = _cfg["prefix_file_name"];
    }

    if (rx_has_key<std::string, std::string>(_cfg, std::string("type"))) {    
        try {
            type = (LogType)std::stoi(_cfg["type"]);
        } catch (const std::exception& e) {
            // Keep default value on conversion error
            type = LOGTYPEDEBUG;
        }
    } 

    if (rx_has_key<std::string, std::string>(_cfg, std::string("model"))) {    
        try {
            model = (LogModel)std::stoi(_cfg["model"]);
        } catch (const std::exception& e) {
            // Keep default value on conversion error
            model = LOGTHREAD;
        }
    } 

    if (rx_has_key<std::string, std::string>(_cfg, std::string("dumppath"))) {
        _dumppath = _cfg["dumppath"];
    }
    
    if (!prefix_file_name.empty() && !log_path.empty()) {
        int i = LOGTYPEFATAL;
        int j = 1;
        for (; i < LOGTYPESIZE; j++, i = 1 << j) {
            get_file_name((LogType)i);
        }
    }
}

bool log_conf::need_reload() {
    struct stat st;

    if (stat(_log_conf_filename.c_str(), &st) == 0 && S_ISREG(st.st_mode) && st.st_mtime != _last_load) {    
        return true;
    }    

    return false;
}

int log_conf::reload() {
    destroy();
    return load();
}

int log_conf::dump() {
    FILE* fp = fopen(_dumppath.c_str(), "w");
    if (!fp) {
        return -1;
    }

    for (auto& ii: _cfg) {
        fprintf(fp, "%s:%s\n", ii.first.c_str(), ii.second.c_str());
    }

    fclose(fp);
    return 0;
}

int log_conf::destroy() {
    _cfg.clear();
    log_path.clear();
    prefix_file_name.clear();
    return 0;
} 
