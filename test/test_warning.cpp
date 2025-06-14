#include "../include/log_helper.h"
#include <iostream>
#include <unistd.h>

using namespace std;

int main() {
    cout << "测试WARNING日志功能..." << endl;
    
    // 初始化日志系统
    LOG_INIT("logger.conf");
    
    // 只写WARNING日志
    cout << "写入WARNING日志..." << endl;
    LOG_WARNING("这是一条WARNING测试日志，错误码: %d", 12345);
    
    cout << "写入流式WARNING日志..." << endl;
    LOGWARNING << "这是一条流式WARNING测试日志: " << 67890;
    
    // 等待异步写入
    cout << "等待写入完成..." << endl;
    sleep(3);
    
    cout << "WARNING日志测试完成" << endl;
    return 0;
} 