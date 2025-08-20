#ifndef __LOGGER_DEF_H__
#define __LOGGER_DEF_H__

#include <sys/socket.h>
#include <sys/epoll.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/time.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <libgen.h>

#include <vector>
#include <string>
#include <list>
#include <map>
#include <set>
#include <deque>
#include <sstream>
#include <exception>
#include <memory>
#include <fstream>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <stdexcept>
#include <inttypes.h>


// Size constants
static const uint32_t SIZE_LEN_4 = 4; 
static const uint32_t SIZE_LEN_6 = 6; 
static const uint32_t SIZE_LEN_8 = 8; 
static const uint32_t SIZE_LEN_10 = 10; 
static const uint32_t SIZE_LEN_12 = 12; 
static const uint32_t SIZE_LEN_16 = 16; 
static const uint32_t SIZE_LEN_32 = 32; 
static const uint32_t SIZE_LEN_64 = 64; 
static const uint32_t SIZE_LEN_128 = 128; 
static const uint32_t SIZE_LEN_256 = 256;
static const uint32_t SIZE_LEN_512 = 512;
static const uint32_t SIZE_LEN_1024 = 1024;
static const uint32_t SIZE_LEN_2048 = 2048;
static const uint32_t SIZE_LEN_4096 = 4096;

// Log specific constants
#define LOG_DATE_FORMAT "%Y-%m-%d %H:%M:%S"
#define DEFAULT_LOG_MAX_SIZE 50*1024*1024
#define CHANNEL_MSG_TAG "c"
#define DAFAULT_EPOLL_SIZE 1000
#define DEFAULT_EPOLL_WAITE 10
#define LOG_ID_MIN 1000

// Platform compatibility
#ifndef MSG_DONTWAIT
#define MSG_DONTWAIT 0x40
#endif

// Type definitions
typedef signed char  int8_t;
typedef unsigned char  uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;

// Split modes
#define SPLIT_MODE_ONE 1
#define SPLIT_MODE_TRIM 2

// Assert macros
#define ASSERT_WARNING(condition, fmt, ...) \
    do { \
        if (!(condition)) { \
            fprintf(stderr, "ASSERT WARNING: " fmt "\n", ##__VA_ARGS__); \
        } \
    } while(0)

#endif 