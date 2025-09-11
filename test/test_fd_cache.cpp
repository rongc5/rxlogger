#include <iostream>
#include <thread>
#include <chrono>
#include <unistd.h>
#include <sys/stat.h>
#include "log_helper.h"

void test_file_deletion_recovery(const std::string& config_file, const std::string& mode_name) {
    std::cout << "\n=== Testing file deletion recovery in " << mode_name << " mode ===" << std::endl;
    
    LOG_INIT(config_file.c_str());
    
    // Write some initial logs
    for (int i = 1; i <= 5; i++) {
        LOG_DEBUG_SAFE("Before deletion - message ", i, " in ", mode_name);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    std::cout << "Initial logs written. Now manually delete logs/*.db files and press Enter...";
    std::cin.get();
    
    // Continue logging after potential file deletion
    for (int i = 6; i <= 10; i++) {
        LOG_DEBUG_SAFE("After deletion - message ", i, " in ", mode_name, " - should recreate files");
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    std::cout << "Completed logging after file deletion. Files should be recreated automatically." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

int main() {
    std::cout << "Testing file descriptor cache behavior with file deletion..." << std::endl;
    std::cout << "This test will demonstrate automatic file recreation when files are deleted." << std::endl;
    
    // Test sync mode
    test_file_deletion_recovery("test_sync_mode.conf", "Sync");
    
    // Small break
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    // Test async mode
    test_file_deletion_recovery("test_async_mode.conf", "Async");
    
    std::cout << "\nFile deletion recovery test completed!" << std::endl;
    std::cout << "Check that log files were recreated after deletion." << std::endl;
    
    return 0;
}