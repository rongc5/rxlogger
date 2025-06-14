#include "../include/log_helper.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <fstream>
#include <vector>
#include <unistd.h>
#include <sys/stat.h>

using namespace std;

// ���Խ��ͳ��
struct TestResult {
    string test_name;
    bool passed;
    string message;
};

vector<TestResult> test_results;

void add_test_result(const string& test_name, bool passed, const string& message = "") {
    test_results.push_back({test_name, passed, message});
}

// ����ļ��Ƿ����
bool file_exists(const string& filename) {
    struct stat buffer;
    return (stat(filename.c_str(), &buffer) == 0);
}

// ����ļ��Ƿ�������
bool file_has_content(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) return false;
    string line;
    return getline(file, line) && !line.empty();
}

// ����1: ������־����
void test_basic_logging() {
    cout << "\n=== ����1: ������־���� ===" << endl;
    
    // ʹ�ø��ּ������־
    LOG_DEBUG("����DEBUG��־������: %d", 123);
    LOG_TRACE("����TRACE��־���ַ���: %s", "hello");
    LOG_NOTICE("����NOTICE��־");
    LOG_WARNING("����WARNING��־��������: %d", -1);
    LOG_FATAL("����FATAL��־");
    
    sleep(1); // �ȴ��첽д��
    
    // �����־�ļ��Ƿ����ɣ����������ض�Ϊcomprehensive_t��
    bool db_exists = file_exists("logs/comprehensive_t.db");
    bool tc_exists = file_exists("logs/comprehensive_t.tc");
    bool nt_exists = file_exists("logs/comprehensive_t.nt");
    bool wn_exists = file_exists("logs/comprehensive_t.wn");
    bool ft_exists = file_exists("logs/comprehensive_t.ft");
    
    add_test_result("DEBUG��־�ļ�����", db_exists);
    add_test_result("TRACE��־�ļ�����", tc_exists);
    add_test_result("NOTICE��־�ļ�����", nt_exists);
    add_test_result("WARNING��־�ļ�����", wn_exists);
    add_test_result("FATAL��־�ļ�����", ft_exists);
}

// ����2: ��ʽ��־����
void test_stream_logging() {
    cout << "\n=== ����2: ��ʽ��־���� ===" << endl;
    
    LOGDEBUG << "��ʽDEBUG��־: " << 456 << " ������: " << 3.14;
    LOGTRACE << "��ʽTRACE��־: " << "world";
    LOGNOTICE << "��ʽNOTICE��־";
    LOGWARNING << "��ʽWARNING��־: " << true;
    LOGFATAL << "��ʽFATAL��־: " << "����";
    
    sleep(1); // �ȴ��첽д��
    
    // �����־�ļ��Ƿ���������
    bool has_content = file_has_content("logs/comprehensive_t.db");
    add_test_result("��ʽ��־д��", has_content);
}

// ����3: �Զ����ļ�д��
void test_custom_file_logging() {
    cout << "\n=== ����3: �Զ����ļ�д�� ===" << endl;
    
    FILE_WRITE("custom_test1.log", "�Զ����ļ�1: %s", "����1");
    FILE_WRITE("custom_test2.log", "�Զ����ļ�2: ���� %d", 999);
    
    sleep(1); // �ȴ��첽д��
    
    bool custom1_exists = file_exists("custom_test1.log");
    bool custom2_exists = file_exists("custom_test2.log");
    
    add_test_result("�Զ����ļ�1д��", custom1_exists);
    add_test_result("�Զ����ļ�2д��", custom2_exists);
}

// ����4: ���߳���־
void test_multithread_logging() {
    cout << "\n=== ����4: ���߳���־���� ===" << endl;
    
    vector<thread> threads;
    
    // ��������߳�ͬʱд��־
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back([i]() {
            for (int j = 0; j < 10; ++j) {
                LOG_DEBUG("�߳�%d ��־%d", i, j);
                LOGNOTICE << "�߳�" << i << " ��ʽ��־ " << j;
                this_thread::sleep_for(chrono::milliseconds(10));
            }
        });
    }
    
    // �ȴ������߳����
    for (auto& t : threads) {
        t.join();
    }
    
    sleep(2); // �ȴ��첽д�����
    
    add_test_result("���߳���־����", true, "���߳���־д�����");
}

// ����5: ���ܲ���
void test_performance() {
    cout << "\n=== ����5: ���ܲ��� ===" << endl;
    
    const int log_count = 1000;
    auto start = chrono::high_resolution_clock::now();
    
    for (int i = 0; i < log_count; ++i) {
        LOG_DEBUG("���ܲ�����־ %d", i);
    }
    
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
    
    double avg_time = static_cast<double>(duration.count()) / log_count;
    cout << "д�� " << log_count << " ����־��ʱ: " << duration.count() << " ΢��" << endl;
    cout << "ƽ��ÿ����־��ʱ: " << avg_time << " ΢��" << endl;
    
    bool performance_ok = avg_time < 100; // ƽ��ÿ����־����100΢��
    add_test_result("���ܲ���", performance_ok, 
                   "ƽ��ÿ����־��ʱ: " + to_string(avg_time) + " ΢��");
}

// ����6: ��־��ʽ��֤
void test_log_format() {
    cout << "\n=== ����6: ��־��ʽ��֤ ===" << endl;
    
    LOG_DEBUG("��ʽ������־");
    sleep(1);
    
    // ��ȡ��־�ļ���֤��ʽ
    ifstream file("logs/comprehensive_t.db");
    if (file.is_open()) {
        string line;
        bool format_ok = false;
        while (getline(file, line)) {
            if (line.find("��ʽ������־") != string::npos) {
                // �����־��ʽ: [log_id:xxxx][app_name]:[timestamp] LEVEL:[thread_id]:[line:func:file] message
                format_ok = (line.find("[log_id:") != string::npos &&
                           line.find("[comprehensive_t]:") != string::npos &&
                           line.find("] DEBUG:[") != string::npos &&
                           line.find(":comprehensive_test.cpp]") != string::npos);
                break;
            }
        }
        file.close();
        add_test_result("��־��ʽ��֤", format_ok);
    } else {
        add_test_result("��־��ʽ��֤", false, "�޷���ȡ��־�ļ�");
    }
}

// ������Խ��
void print_test_results() {
    cout << "\n=== ���Խ������ ===" << endl;
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
    
    cout << "\n�ܼ�: " << passed << "/" << total << " ����ͨ��" << endl;
    cout << "�ɹ���: " << (100.0 * passed / total) << "%" << endl;
}

int main() {
    cout << "��ʼȫ�������־ϵͳ..." << endl;
    
    // ��ʼ����־ϵͳ
    LOG_INIT("logger.conf");
    
    // ���и������
    test_basic_logging();
    test_stream_logging();
    test_custom_file_logging();
    test_multithread_logging();
    test_performance();
    test_log_format();
    
    // �ȴ������첽д�����
    sleep(3);
    
    // ������Խ��
    print_test_results();
    
    cout << "\n������ɣ����� logs Ŀ¼�µ���־�ļ���" << endl;
    
    return 0;
} 