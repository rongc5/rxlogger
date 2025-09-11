#include <iostream>
#include <thread>
#include <chrono>
#include "log_helper.h"

int main() {
    std::cout << "Testing rename check optimization..." << std::endl;
    std::cout << "Config: rename_check_every=5, rename_check_interval_ms=1000, file_max_size=1024" << std::endl;
    
    LOG_INIT("test_rename_check.conf");
    
    std::cout << "Writing messages to trigger file rotation..." << std::endl;
    
    // 写入足够多的数据触发文件轮转
    for (int i = 1; i <= 50; i++) {
        LOG_DEBUG_SAFE("Long debug message ", i, " - this message is designed to be long enough to trigger file size limits and test the rename checking optimization. The message contains enough text to reach the file size limit quickly.");
        
        if (i % 10 == 0) {
            std::cout << "Written " << i << " messages..." << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
        
        // 每5条消息应该检查一次rename (由于配置了rename_check_every=5)
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    // 等待异步处理完成
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    std::cout << "\nRename optimization test completed!" << std::endl;
    std::cout << "Check logs directory for rotated files (should see rename_test.db.YYYYMMDDHHMMSS files)" << std::endl;
    
    return 0;
}