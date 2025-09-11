#include <iostream>
#include <thread>
#include <chrono>
#include <unistd.h>
#include <signal.h>
#include "log_helper.h"

volatile bool keep_running = true;

void signal_handler(int signal) {
    keep_running = false;
    std::cout << "\nReceived signal " << signal << ", shutting down..." << std::endl;
}

int main() {
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    // Initialize logger
    LOG_INIT("test/log.conf");
    
    std::cout << "Starting file deletion test..." << std::endl;
    std::cout << "Press Ctrl+C to stop" << std::endl;
    std::cout << "While running, try deleting files from test/logs/ directory" << std::endl;
    
    int counter = 0;
    
    while (keep_running) {
        // Log some messages
        LOG_DEBUG_SAFE("Test message ", counter, " - checking file deletion resilience");
        LOG_NOTICE_SAFE("Counter: ", counter, ", timestamp: ", time(nullptr));
        LOG_WARNING_SAFE("This is a warning message #", counter);
        
        // Log to custom file
        FILE_WRITE_SAFE("test/custom_deletion_test.log", 
                       "Custom file message ", counter);
        
        counter++;
        
        // Print status every 50 messages
        if (counter % 50 == 0) {
            std::cout << "Logged " << counter << " messages. Log files should be in test/logs/" << std::endl;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    std::cout << "Test completed. Total messages logged: " << counter << std::endl;
    return 0;
}