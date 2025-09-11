#include <iostream>
#include <thread>
#include <chrono>
#include <unistd.h>
#include "log_helper.h"

void test_logging_mode(const std::string& config_file, const std::string& mode_name) {
    std::cout << "\n=== Testing " << mode_name << " mode ===" << std::endl;
    
    // Initialize logger
    LOG_INIT(config_file.c_str());
    
    std::cout << "Logging 20 messages..." << std::endl;
    
    for (int i = 1; i <= 20; i++) {
        LOG_DEBUG_SAFE("Debug message ", i, " in ", mode_name, " mode");
        LOG_NOTICE_SAFE("Notice message ", i, " timestamp: ", time(nullptr));
        LOG_WARNING_SAFE("Warning message ", i, " - testing file caching");
        
        // Test custom file logging
        FILE_WRITE_SAFE("test_custom.log", "Custom file message ", i, " in ", mode_name);
        
        if (i % 5 == 0) {
            std::cout << "Logged " << i << " rounds..." << std::endl;
        }
        
        // Small delay to see timing differences
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    std::cout << mode_name << " mode test completed!" << std::endl;
    
    // Give time for async mode to flush
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

int main() {
    std::cout << "Testing both logging modes with file descriptor caching..." << std::endl;
    
    // Test sync mode (model=1)
    test_logging_mode("test_sync_mode.conf", "Sync");
    
    // Wait a bit between tests
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    // Test async mode (model=2) 
    test_logging_mode("test_async_mode.conf", "Async");
    
    std::cout << "\nBoth tests completed. Check logs directory for output files." << std::endl;
    std::cout << "Files should be:" << std::endl;
    std::cout << "- logs/sync_test.debug" << std::endl;
    std::cout << "- logs/sync_test.notice" << std::endl; 
    std::cout << "- logs/sync_test.warning" << std::endl;
    std::cout << "- logs/async_test.debug" << std::endl;
    std::cout << "- logs/async_test.notice" << std::endl;
    std::cout << "- logs/async_test.warning" << std::endl;
    std::cout << "- test_custom.log" << std::endl;
    
    return 0;
}