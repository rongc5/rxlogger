#include "../include/log_helper.h"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    // Use a clean ASCII-only config and prefix
    LOG_INIT("test/style_logger.conf");

    // printf-style with prefix (default LOG_*)
    LOG_DEBUG("printf number=%d text=%s", 7, "ok");

    // stream-style
    LOGDEBUG << "stream number=" << 9 << " text=" << "ok";

    // file printf-style
    FILE_WRITE("style_custom.log", "file printf a=%d b=%s", 5, "ok");

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return 0;
}

