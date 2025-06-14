#include "../include/log_helper.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>

using namespace std;

// ģ��ҵ����
void process_order(int order_id, double amount) {
    LOG_NOTICE("��ʼ������: %d, ���: %.2f", order_id, amount);
    
    // ģ�⴦��ʱ��
    this_thread::sleep_for(chrono::milliseconds(100));
    
    if (amount > 1000) {
        LOG_WARNING("�߶����Ҫ���: %d, ���: %.2f", order_id, amount);
    }
    
    LOGDEBUG << "�������� - ID: " << order_id << ", ���: " << amount;
    
    // ģ����ܵĴ���
    if (order_id % 100 == 0) {
        LOG_FATAL("��������ʧ��: %d - ���ݿ����ӳ�ʱ", order_id);
        return;
    }
    
    LOG_NOTICE("�����������: %d", order_id);
    
    // д�������־
    FILE_WRITE("audit.log", "���� %d ������ɣ����: %.2f, ʱ��: %ld",
               order_id, amount, time(nullptr));
}

// ģ���û���¼
void user_login(const string& username) {
    LOG_TRACE("�û���¼��ʼ: %s", username.c_str());
    
    if (username == "admin") {
        LOGNOTICE << "����Ա��¼: " << username;
    } else {
        LOGDEBUG << "��ͨ�û���¼: " << username;
    }
    
    LOG_TRACE("�û���¼���: %s", username.c_str());
}

// ���ܲ��Ժ���
void performance_test() {
    cout << "��ʼ���ܲ���..." << endl;
    
    auto start = chrono::high_resolution_clock::now();
    
    // д�������־��������
    for (int i = 0; i < 10000; ++i) {
        LOG_DEBUG("���ܲ�����־ %d", i);
    }
    
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
    
    cout << "д��10000����־��ʱ: " << duration.count() << " ΢��" << endl;
    cout << "ƽ��ÿ����־: " << (double)duration.count()/10000 << " ΢��" << endl;
    
    LOG_NOTICE("���ܲ�����ɣ��ܺ�ʱ: %ld ΢��", duration.count());
}

// ���̲߳���
void multithread_test() {
    cout << "��ʼ���̲߳���..." << endl;
    
    vector<thread> threads;
    
    // ��������̲߳���д��־
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back([i]() {
            for (int j = 0; j < 20; ++j) {
                LOG_DEBUG("�߳� %d �������� %d", i, j);
                LOGNOTICE << "�߳� " << i << " ������� " << j;
                this_thread::sleep_for(chrono::milliseconds(10));
            }
        });
    }
    
    // �ȴ������߳����
    for (auto& t : threads) {
        t.join();
    }
    
    LOG_NOTICE("���̲߳������");
}

int main() {
    cout << "=== Logger��־��ʹ��ʾ�� ===" << endl;
    
    try {
        // ��ʼ����־ϵͳ
        LOG_INIT("../test/logger.conf");
        cout << "��־ϵͳ�������" << endl;
        
        // 1. ������־��¼
        cout << "\n1. ������־��¼����..." << endl;
        LOG_DEBUG("�����������汾: %s", "1.0.0");
        LOG_NOTICE("ϵͳ��ʼ�����");
        
        // 2. ҵ�񳡾�����
        cout << "\n2. ҵ�񳡾�����..." << endl;
        user_login("admin");
        user_login("user001");
        
        process_order(1001, 299.99);
        process_order(1002, 1599.99);  // �߶��
        process_order(1100, 99.99);    // �ᴥ������
        
        // 3. ��ʽ��־����
        cout << "\n3. ��ʽ��־����..." << endl;
        LOGDEBUG << "��ǰʱ���: " << time(nullptr);
        LOGWARNING << "�ڴ�ʹ����: " << 85.5 << "%";
        
        // 4. ���ܲ���
        cout << "\n4. ���ܲ���..." << endl;
        performance_test();
        
        // 5. ���̲߳���
        cout << "\n5. ���̲߳���..." << endl;
        multithread_test();
        
        // �ȴ��첽��־д�����
        cout << "\n�ȴ���־д�����..." << endl;
        this_thread::sleep_for(chrono::seconds(2));
        
        LOG_NOTICE("ʾ�������������");
        cout << "\nʾ������������ɣ���鿴logsĿ¼�µ���־�ļ���" << endl;
        
    } catch (const exception& e) {
        cerr << "�������г���: " << e.what() << endl;
        return 1;
    }
    
    return 0;
} 