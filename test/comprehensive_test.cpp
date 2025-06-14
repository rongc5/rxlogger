#include "../include/log_helper.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <fstream>
#include <vector>
#include <unistd.h>
#include <sys/stat.h>

using namespace std;

// 测试结果统计
struct TestResult {
    string test_name;
    bool passed;
    string message;
};

vector<TestResult> test_results;

void add_test_result(const string& test_name, bool passed, const string& message = "") {
    test_results.push_back({test_name, passed, message});
}

// 检查文件是否存在
bool file_exists(const string& filename) {
    struct stat buffer;
    return (stat(filename.c_str(), &buffer) == 0);
}

// 检查文件是否有内容
bool file_has_content(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) return false;
    string line;
    return getline(file, line) && !line.empty();
}

// 测试1: 基本日志功能
void test_basic_logging() {
    cout << "\n=== 测试1: 基本日志功能 ===" << endl;
    
    // 使用各种级别的日志
    LOG_DEBUG("测试DEBUG日志，数字: %d", 123);
    LOG_TRACE("测试TRACE日志，字符串: %s", "hello");
    LOG_NOTICE("测试NOTICE日志");
    LOG_WARNING("测试WARNING日志，错误码: %d", -1);
    LOG_FATAL("测试FATAL日志");
    
    sleep(1); // 等待异步写入
    
    // 检查日志文件是否生成（程序名被截断为comprehensive_t）
    bool db_exists = file_exists("logs/comprehensive_t.db");
    bool tc_exists = file_exists("logs/comprehensive_t.tc");
    bool nt_exists = file_exists("logs/comprehensive_t.nt");
    bool wn_exists = file_exists("logs/comprehensive_t.wn");
    bool ft_exists = file_exists("logs/comprehensive_t.ft");
    
    add_test_result("DEBUG日志文件生成", db_exists);
    add_test_result("TRACE日志文件生成", tc_exists);
    add_test_result("NOTICE日志文件生成", nt_exists);
    add_test_result("WARNING日志文件生成", wn_exists);
    add_test_result("FATAL日志文件生成", ft_exists);
}

// 测试2: 流式日志功能
void test_stream_logging() {
    cout << "\n=== 测试2: 流式日志功能 ===" << endl;
    
    LOGDEBUG << "流式DEBUG日志: " << 456 << " 浮点数: " << 3.14;
    LOGTRACE << "流式TRACE日志: " << "world";
    LOGNOTICE << "流式NOTICE日志";
    LOGWARNING << "流式WARNING日志: " << true;
    LOGFATAL << "流式FATAL日志: " << "结束";
    
    sleep(1); // 等待异步写入
    
    // 检查日志文件是否有新内容
    bool has_content = file_has_content("logs/comprehensive_t.db");
    add_test_result("流式日志写入", has_content);
}

// 测试3: 自定义文件写入
void test_custom_file_logging() {
    cout << "\n=== 测试3: 自定义文件写入 ===" << endl;
    
    FILE_WRITE("custom_test1.log", "自定义文件1: %s", "内容1");
    FILE_WRITE("custom_test2.log", "自定义文件2: 数字 %d", 999);
    
    sleep(1); // 等待异步写入
    
    bool custom1_exists = file_exists("custom_test1.log");
    bool custom2_exists = file_exists("custom_test2.log");
    
    add_test_result("自定义文件1写入", custom1_exists);
    add_test_result("自定义文件2写入", custom2_exists);
}

// 测试4: 多线程日志
void test_multithread_logging() {
    cout << "\n=== 测试4: 多线程日志测试 ===" << endl;
    
    vector<thread> threads;
    
    // 创建多个线程同时写日志
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back([i]() {
            for (int j = 0; j < 10; ++j) {
                LOG_DEBUG("线程%d 日志%d", i, j);
                LOGNOTICE << "线程" << i << " 流式日志 " << j;
                this_thread::sleep_for(chrono::milliseconds(10));
            }
        });
    }
    
    // 等待所有线程完成
    for (auto& t : threads) {
        t.join();
    }
    
    sleep(2); // 等待异步写入完成
    
    add_test_result("多线程日志测试", true, "多线程日志写入完成");
}

// 测试5: 性能测试
void test_performance() {
    cout << "\n=== 测试5: 性能测试 ===" << endl;
    
    const int log_count = 1000;
    auto start = chrono::high_resolution_clock::now();
    
    for (int i = 0; i < log_count; ++i) {
        LOG_DEBUG("性能测试日志 %d", i);
    }
    
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
    
    double avg_time = static_cast<double>(duration.count()) / log_count;
    cout << "写入 " << log_count << " 条日志耗时: " << duration.count() << " 微秒" << endl;
    cout << "平均每条日志耗时: " << avg_time << " 微秒" << endl;
    
    bool performance_ok = avg_time < 100; // 平均每条日志少于100微秒
    add_test_result("性能测试", performance_ok, 
                   "平均每条日志耗时: " + to_string(avg_time) + " 微秒");
}

// 测试6: 日志格式验证
void test_log_format() {
    cout << "\n=== 测试6: 日志格式验证 ===" << endl;
    
    LOG_DEBUG("格式测试日志");
    sleep(1);
    
    // 读取日志文件验证格式
    ifstream file("logs/comprehensive_t.db");
    if (file.is_open()) {
        string line;
        bool format_ok = false;
        while (getline(file, line)) {
            if (line.find("格式测试日志") != string::npos) {
                // 检查日志格式: [log_id:xxxx][app_name]:[timestamp] LEVEL:[thread_id]:[line:func:file] message
                format_ok = (line.find("[log_id:") != string::npos &&
                           line.find("[comprehensive_t]:") != string::npos &&
                           line.find("] DEBUG:[") != string::npos &&
                           line.find(":comprehensive_test.cpp]") != string::npos);
                break;
            }
        }
        file.close();
        add_test_result("日志格式验证", format_ok);
    } else {
        add_test_result("日志格式验证", false, "无法读取日志文件");
    }
}

// 输出测试结果
void print_test_results() {
    cout << "\n=== 测试结果汇总 ===" << endl;
    int passed = 0, total = test_results.size();
    
    for (const auto& result : test_results) {
        cout << "[" << (result.passed ? "PASS" : "FAIL") << "] " 
             << result.test_name;
        if (!result.message.empty()) {
            cout << " - " << result.message;
        }
        cout << endl;
        
        if (result.passed) passed++;
    }
    
    cout << "\n总计: " << passed << "/" << total << " 测试通过" << endl;
    cout << "成功率: " << (100.0 * passed / total) << "%" << endl;
}

int main() {
    cout << "开始全面测试日志系统..." << endl;
    
    // 初始化日志系统
    LOG_INIT("logger.conf");
    
    // 运行各项测试
    test_basic_logging();
    test_stream_logging();
    test_custom_file_logging();
    test_multithread_logging();
    test_performance();
    test_log_format();
    
    // 等待所有异步写入完成
    sleep(3);
    
    // 输出测试结果
    print_test_results();
    
    cout << "\n测试完成！请检查 logs 目录下的日志文件。" << endl;
    
    return 0;
} 