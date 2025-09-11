#include <iostream>
#include <thread>
#include <chrono>
#include "log_helper.h"

void test_write_mode(const std::string& config_file, const std::string& mode_name) {
    std::cout << "\n=== Testing " << mode_name << " write mode ===" << std::endl;
    
    LOG_INIT(config_file.c_str());
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // 写入大量日志测试性能
    for (int i = 1; i <= 100; i++) {
        LOG_DEBUG_SAFE("Debug message ", i, " in ", mode_name, " mode - performance test");
        LOG_NOTICE_SAFE("Notice ", i, " with timestamp ", time(nullptr), " in ", mode_name);
        LOG_WARNING_SAFE("Warning ", i, " - testing writev vs fprintf performance");
        
        // 测试自定义文件
        std::string custom_file = "custom_" + mode_name + ".log";
        FILE_WRITE_SAFE(custom_file.c_str(), 
                       "Custom message ", i, " using ", mode_name, " mode");
        
        if (i % 25 == 0) {
            std::cout << "Processed " << i << " rounds..." << std::endl;
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << mode_name << " mode completed in " << duration.count() << " ms" << std::endl;
    
    // 等待异步写入完成
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

int main() {
    std::cout << "Comparing Direct Write vs Stdio performance..." << std::endl;
    
    // 测试Direct Write模式 (writev)
    test_write_mode("test_direct_write.conf", "Direct");
    
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    // 测试Stdio模式 (fprintf) 
    test_write_mode("test_stdio_mode.conf", "Stdio");
    
    std::cout << "\nPerformance comparison completed!" << std::endl;
    std::cout << "Files created:" << std::endl;
    std::cout << "- logs/direct_test.* (using writev)" << std::endl;
    std::cout << "- logs/stdio_test.* (using fprintf)" << std::endl;
    std::cout << "- custom_Direct.log" << std::endl;
    std::cout << "- custom_Stdio.log" << std::endl;
    
    return 0;
}