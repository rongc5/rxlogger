#include <iostream>
#include <thread>
#include <chrono>
#include "log_helper.h"

int main() {
    std::cout << "Testing rename check optimization..." << std::endl;
    std::cout << "Small file size (1KB) should trigger rotation quickly" << std::endl;
    
    LOG_INIT("test_rename_check.conf");
    
    // 写入长消息快速达到文件大小限制
    for (int i = 1; i <= 30; i++) {
        LOG_DEBUG_SAFE("Message ", i, " - Long text to fill file quickly and test rename optimization with reduced stat calls. This should trigger file rotation when size exceeds 1024 bytes.");
        
        if (i % 5 == 0) {
            std::cout << "Written " << i << " messages, checking for rotated files..." << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
        }
    }
    
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    std::cout << "Test completed. Check logs/ for rotated files." << std::endl;
    return 0;
}