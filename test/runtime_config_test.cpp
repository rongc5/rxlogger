#include "../include/log_helper.h"
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <chrono>
#include <thread>

using namespace std;

void show_current_config() {
    log_thread* thread = base_singleton<log_thread>::get_instance_ex();
    if (thread) {
        cout << "当前日志级别支持状态：" << endl;
        cout << "  FATAL   (1): " << (thread->check_type(LOGTYPEFATAL)   ? "启用" : "禁用") << endl;
        cout << "  WARNING (2): " << (thread->check_type(LOGTYPEWARNING) ? "启用" : "禁用") << endl;
        cout << "  NOTICE  (4): " << (thread->check_type(LOGTYPENOTICE)  ? "启用" : "禁用") << endl;
        cout << "  TRACE   (8): " << (thread->check_type(LOGTYPETRACE)   ? "启用" : "禁用") << endl;
        cout << "  DEBUG  (16): " << (thread->check_type(LOGTYPEDEBUG)   ? "启用" : "禁用") << endl;
    }
}

void modify_config_file(int log_level) {
    // 修改配置文件
    ofstream conf("logger.conf");
    conf << "# 动态修改的配置文件\n";
    conf << "file_max_size=10485760\n";
    conf << "log_path=logs\n";
    conf << "prefix_file_name=runtime_test\n";
    conf << "type=" << log_level << "\n";  // 动态修改日志级别
    conf << "model=2\n";
    conf << "dumppath=log_conf_dump\n";
    conf.close();
    
    cout << "配置文件已修改，日志级别设置为: " << log_level << endl;
}

void test_logging() {
    cout << "测试各级别日志写入..." << endl;
    LOG_DEBUG("DEBUG日志测试");
    LOG_TRACE("TRACE日志测试");
    LOG_NOTICE("NOTICE日志测试");
    LOG_WARNING("WARNING日志测试");
    LOG_FATAL("FATAL日志测试");
    cout << "日志写入完成" << endl;
}

int main() {
    cout << "=== 运行时配置修改测试 ===" << endl;
    
    // 1. 初始配置：启用所有级别
    modify_config_file(31);  // 31 = 1+2+4+8+16 (所有级别)
    LOG_INIT("logger.conf");
    
    cout << "\n阶段1: 启用所有日志级别 (type=31)" << endl;
    show_current_config();
    test_logging();
    
    // 等待一下，确保配置文件时间戳发生变化
    sleep(1);
    
    // 2. 运行时修改：只启用关键级别
    cout << "\n阶段2: 修改配置文件，只启用关键级别..." << endl;
    modify_config_file(7);   // 7 = 1+2+4 (FATAL+WARNING+NOTICE)
    
    // 等待配置重载（日志系统会在下次处理时检查配置文件）
    cout << "等待配置重载..." << endl;
    LOG_NOTICE("触发配置检查的日志");  // 这会触发配置检查
    sleep(2);  // 给一点时间让配置重载
    
    cout << "\n配置重载后的状态：" << endl;
    show_current_config();
    test_logging();
    
    // 等待一下
    sleep(1);
    
    // 3. 再次修改：只启用DEBUG级别
    cout << "\n阶段3: 再次修改配置，只启用DEBUG级别..." << endl;
    modify_config_file(16);  // 16 = 只有DEBUG级别
    
    cout << "等待配置重载..." << endl;
    LOG_DEBUG("触发配置检查的DEBUG日志");
    sleep(2);
    
    cout << "\n再次重载后的状态：" << endl;
    show_current_config();
    test_logging();
    
    // 4. 最终测试：恢复全部级别
    cout << "\n阶段4: 恢复全部级别..." << endl;
    modify_config_file(31);
    
    LOG_NOTICE("最终测试触发");
    sleep(2);
    
    cout << "\n最终状态：" << endl;
    show_current_config();
    test_logging();
    
    cout << "\n=== 运行时配置修改测试完成 ===" << endl;
    cout << "检查 logs/ 目录查看不同阶段生成的日志文件" << endl;
    
    return 0;
} 