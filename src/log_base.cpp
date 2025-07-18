#include "../include/log_base.h"

log_conf::log_conf(const char* sk_conf) {
    _log_conf_filename.append(sk_conf);  
    _last_load = 0;
}

int log_conf::load() {
    FILE* fp = fopen(_log_conf_filename.c_str(), "r");
    ASSERT_WARNING(fp != NULL, "open log conf failed. path[%s]", _log_conf_filename.c_str());
    
    if (!fp) {
        return -1;
    }

    char line[SIZE_LEN_1024];
    char* ptr = NULL;
    vector<string> tmp_vec;
    
    while (fgets(line, 1024, fp)) {
        if('\0' == line[0]) {
            continue;
        }

        ptr = im_chomp(line);
        ptr = im_chomp(ptr, '#');

        if (ptr == NULL || *ptr == '\0'|| *ptr == '#') {
            continue;
        }

        SplitString(ptr, "=", &tmp_vec, SPLIT_MODE_ONE | SPLIT_MODE_TRIM);
        if (tmp_vec.size() != 2) {
            continue;
        }

        char* key = trim(tmp_vec[0].c_str());
        char* value = trim(tmp_vec[1].c_str());
        
        if (key && value) {
            _cfg.insert(make_pair(string(key), string(value)));
            free(key);
            free(value);
        }
    }

    fclose(fp);
    do_parse();

    struct stat st;
    stat(_log_conf_filename.c_str(), &st);
    _last_load = st.st_mtime;

    return 0;
}

void log_conf::get_file_name(LogType type) {
    stringstream ss;

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
    char buf[SIZE_LEN_1024];
    get_proc_name(buf, sizeof(buf));
    prefix_file_name.assign(buf);

    file_max_size = DEFAULT_LOG_MAX_SIZE;
    type = LOGTYPEDEBUG;
    log_path.assign("logs");
    model = LOGTHREAD;
    _dumppath.assign("log_conf_dump");

    if (has_key<string, string>(_cfg, "file_max_size")) {    
        file_max_size = atoi(_cfg["file_max_size"].c_str());
    } 

    if (has_key<string, string>(_cfg, "log_path")) {
        log_path = _cfg["log_path"];
    }

    if (has_key<string, string>(_cfg, "prefix_file_name")) {
        prefix_file_name = _cfg["prefix_file_name"];
    }

    if (has_key<string, string>(_cfg, "type")) {    
        type = (LogType)atoi(_cfg["type"].c_str());
    } 

    if (has_key<string, string>(_cfg, "model")) {    
        model = (LogModel)atoi(_cfg["model"].c_str());
    } 

    if (has_key<string, string>(_cfg, "dumppath")) {
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