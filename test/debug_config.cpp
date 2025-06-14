#include "../include/log_helper.h"
#include "../include/log_base.h"
#include <iostream>

using namespace std;

int main() {
    cout << "=== 日志配置调试工具 ===" << endl;
    
    // 初始化日志系统
    LOG_INIT("logger.conf");
    
    // 获取日志系统实例
    log_thread* thread = base_singleton<log_thread>::get_instance_ex();
    if (!thread) {
        cout << "错误：无法获取日志线程实例" << endl;
        return 1;
    }
    
    // 检查各种日志类型
    cout << "检查日志类型支持状态：" << endl;
    cout << "FATAL   (" << LOGTYPEFATAL   << "): " << (thread->check_type(LOGTYPEFATAL)   ? "启用" : "禁用") << endl;
    cout << "WARNING (" << LOGTYPEWARNING << "): " << (thread->check_type(LOGTYPEWARNING) ? "启用" : "禁用") << endl;
    cout << "NOTICE  (" << LOGTYPENOTICE  << "): " << (thread->check_type(LOGTYPENOTICE)  ? "启用" : "禁用") << endl;
    cout << "TRACE   (" << LOGTYPETRACE   << "): " << (thread->check_type(LOGTYPETRACE)   ? "启用" : "禁用") << endl;
    cout << "DEBUG   (" << LOGTYPEDEBUG   << "): " << (thread->check_type(LOGTYPEDEBUG)   ? "启用" : "禁用") << endl;
    
    // 测试WARNING日志写入
    cout << "\n写入WARNING日志测试..." << endl;
    LOG_WARNING("调试用WARNING日志");
    
    sleep(2);
    cout << "调试完成" << endl;
    
    return 0;
} 