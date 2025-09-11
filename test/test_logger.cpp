#include "../include/log_helper.h"
#include <iostream>
#include <unistd.h>

using namespace std;

int main(int argc, char* argv[]) {
    cout << "开始测试日志系统..." << endl;
    
    // 测试1: 初始化日志系统
    const char* log_conf_path = "test/logger.conf";
    LOG_INIT(log_conf_path);
    
    cout << "日志系统初始化完成" << endl;
    
    // 测试2: 使用不同级别的日志
    LOG_DEBUG("这是一条调试日志，数值: %d", 42);
    LOG_TRACE("这是一条跟踪日志，字符串: %s", "测试");
    LOG_NOTICE("这是一条通知日志");
    LOG_WARNING("这是一条警告日志，错误码: %d", -1);
    LOG_FATAL("这是一条致命错误日志");
    
    // 测试3: 使用流式日志
    LOGDEBUG << "流式调试日志: " << 123 << " 字符串: " << "测试";
    LOGTRACE << "流式跟踪日志: " << 3.14;
    LOGNOTICE << "流式通知日志";
    LOGWARNING << "流式警告日志";
    LOGFATAL << "流式致命错误日志";
    
    // 测试4: 写入特定文件
    FILE_WRITE("test_custom.log", "自定义文件日志: %s", "测试内容");
    
    cout << "等待日志写入完成..." << endl;
    sleep(2);
    
    cout << "日志测试完成，请检查 logs 目录下的日志文件" << endl;
    
    return 0;
} 