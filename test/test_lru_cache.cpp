#include <iostream>
#include <thread>
#include <chrono>
#include "log_helper.h"

int main() {
    std::cout << "Testing LRU cache with capacity=3..." << std::endl;
    
    LOG_INIT("test_lru_config.conf");
    
    // 创建多个文件，超过缓存容量
    std::cout << "Writing to 5 different files (cache capacity=3):" << std::endl;
    
    for (int round = 1; round <= 3; round++) {
        std::cout << "\n--- Round " << round << " ---" << std::endl;
        
        // 写入5个不同文件，应该触发LRU淘汰
        FILE_WRITE_SAFE("file1.log", "Round ", round, " message to file1");
        std::cout << "Wrote to file1.log" << std::endl;
        
        FILE_WRITE_SAFE("file2.log", "Round ", round, " message to file2");  
        std::cout << "Wrote to file2.log" << std::endl;
        
        FILE_WRITE_SAFE("file3.log", "Round ", round, " message to file3");
        std::cout << "Wrote to file3.log" << std::endl;
        
        FILE_WRITE_SAFE("file4.log", "Round ", round, " message to file4");
        std::cout << "Wrote to file4.log (should evict file1)" << std::endl;
        
        FILE_WRITE_SAFE("file5.log", "Round ", round, " message to file5");
        std::cout << "Wrote to file5.log (should evict file2)" << std::endl;
        
        // 再次写入file1，应该重新打开
        FILE_WRITE_SAFE("file1.log", "Round ", round, " message to file1 again");
        std::cout << "Wrote to file1.log again (should evict file3)" << std::endl;
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    // 测试常规日志文件的LRU
    std::cout << "\nTesting regular log files:" << std::endl;
    for (int i = 1; i <= 10; i++) {
        LOG_DEBUG_SAFE("Debug message ", i);
        LOG_NOTICE_SAFE("Notice message ", i);
        LOG_WARNING_SAFE("Warning message ", i);
        
        if (i % 3 == 0) {
            std::cout << "Logged " << i << " rounds to regular files" << std::endl;
        }
    }
    
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::cout << "\nLRU cache test completed!" << std::endl;
    std::cout << "Check that all files were created and contain expected messages." << std::endl;
    
    return 0;
}