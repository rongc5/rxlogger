#include "../include/log_helper.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>

using namespace std;

// 模拟业务函数
void process_order(int order_id, double amount) {
    LOG_NOTICE("开始处理订单: %d, 金额: %.2f", order_id, amount);
    
    // 模拟处理时间
    this_thread::sleep_for(chrono::milliseconds(100));
    
    if (amount > 1000) {
        LOG_WARNING("高额订单需要审核: %d, 金额: %.2f", order_id, amount);
    }
    
    LOGDEBUG << "订单详情 - ID: " << order_id << ", 金额: " << amount;
    
    // 模拟可能的错误
    if (order_id % 100 == 0) {
        LOG_FATAL("订单处理失败: %d - 数据库连接超时", order_id);
        return;
    }
    
    LOG_NOTICE("订单处理完成: %d", order_id);
    
    // 写入审计日志
    FILE_WRITE("audit.log", "订单 %d 处理完成，金额: %.2f, 时间: %ld",
               order_id, amount, time(nullptr));
}

// 模拟用户登录
void user_login(const string& username) {
    LOG_TRACE("用户登录开始: %s", username.c_str());
    
    if (username == "admin") {
        LOGNOTICE << "管理员登录: " << username;
    } else {
        LOGDEBUG << "普通用户登录: " << username;
    }
    
    LOG_TRACE("用户登录完成: %s", username.c_str());
}

// 性能测试函数
void performance_test() {
    cout << "开始性能测试..." << endl;
    
    auto start = chrono::high_resolution_clock::now();
    
    // 写入大量日志测试性能
    for (int i = 0; i < 10000; ++i) {
        LOG_DEBUG("性能测试日志 %d", i);
    }
    
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
    
    cout << "写入10000条日志耗时: " << duration.count() << " 微秒" << endl;
    cout << "平均每条日志: " << (double)duration.count()/10000 << " 微秒" << endl;
    
    LOG_NOTICE("性能测试完成，总耗时: %ld 微秒", duration.count());
}

// 多线程测试
void multithread_test() {
    cout << "开始多线程测试..." << endl;
    
    vector<thread> threads;
    
    // 创建多个线程并发写日志
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back([i]() {
            for (int j = 0; j < 20; ++j) {
                LOG_DEBUG("线程 %d 处理任务 %d", i, j);
                LOGNOTICE << "线程 " << i << " 完成任务 " << j;
                this_thread::sleep_for(chrono::milliseconds(10));
            }
        });
    }
    
    // 等待所有线程完成
    for (auto& t : threads) {
        t.join();
    }
    
    LOG_NOTICE("多线程测试完成");
}

int main() {
    cout << "=== Logger日志库使用示例 ===" << endl;
    
    try {
        // 初始化日志系统
        LOG_INIT("../test/logger.conf");
        cout << "日志系统初化完成" << endl;
        
        // 1. 基本日志记录
        cout << "\n1. 基本日志记录测试..." << endl;
        LOG_DEBUG("程序启动，版本: %s", "1.0.0");
        LOG_NOTICE("系统初始化完成");
        
        // 2. 业务场景测试
        cout << "\n2. 业务场景测试..." << endl;
        user_login("admin");
        user_login("user001");
        
        process_order(1001, 299.99);
        process_order(1002, 1599.99);  // 高额订单
        process_order(1100, 99.99);    // 会触发错误
        
        // 3. 流式日志测试
        cout << "\n3. 流式日志测试..." << endl;
        LOGDEBUG << "当前时间戳: " << time(nullptr);
        LOGWARNING << "内存使用率: " << 85.5 << "%";
        
        // 4. 性能测试
        cout << "\n4. 性能测试..." << endl;
        performance_test();
        
        // 5. 多线程测试
        cout << "\n5. 多线程测试..." << endl;
        multithread_test();
        
        // 等待异步日志写入完成
        cout << "\n等待日志写入完成..." << endl;
        this_thread::sleep_for(chrono::seconds(2));
        
        LOG_NOTICE("示例程序运行完成");
        cout << "\n示例程序运行完成！请查看logs目录下的日志文件。" << endl;
        
    } catch (const exception& e) {
        cerr << "程序运行出错: " << e.what() << endl;
        return 1;
    }
    
    return 0;
} 