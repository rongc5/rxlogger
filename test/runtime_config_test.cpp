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
        cout << "��ǰ��־����֧��״̬��" << endl;
        cout << "  FATAL   (1): " << (thread->check_type(LOGTYPEFATAL)   ? "����" : "����") << endl;
        cout << "  WARNING (2): " << (thread->check_type(LOGTYPEWARNING) ? "����" : "����") << endl;
        cout << "  NOTICE  (4): " << (thread->check_type(LOGTYPENOTICE)  ? "����" : "����") << endl;
        cout << "  TRACE   (8): " << (thread->check_type(LOGTYPETRACE)   ? "����" : "����") << endl;
        cout << "  DEBUG  (16): " << (thread->check_type(LOGTYPEDEBUG)   ? "����" : "����") << endl;
    }
}

void modify_config_file(int log_level) {
    // �޸������ļ�
    ofstream conf("logger.conf");
    conf << "# ��̬�޸ĵ������ļ�\n";
    conf << "file_max_size=10485760\n";
    conf << "log_path=logs\n";
    conf << "prefix_file_name=runtime_test\n";
    conf << "type=" << log_level << "\n";  // ��̬�޸���־����
    conf << "model=2\n";
    conf << "dumppath=log_conf_dump\n";
    conf.close();
    
    cout << "�����ļ����޸ģ���־��������Ϊ: " << log_level << endl;
}

void test_logging() {
    cout << "���Ը�������־д��..." << endl;
    LOG_DEBUG("DEBUG��־����");
    LOG_TRACE("TRACE��־����");
    LOG_NOTICE("NOTICE��־����");
    LOG_WARNING("WARNING��־����");
    LOG_FATAL("FATAL��־����");
    cout << "��־д�����" << endl;
}

int main() {
    cout << "=== ����ʱ�����޸Ĳ��� ===" << endl;
    
    // 1. ��ʼ���ã��������м���
    modify_config_file(31);  // 31 = 1+2+4+8+16 (���м���)
    LOG_INIT("logger.conf");
    
    cout << "\n�׶�1: ����������־���� (type=31)" << endl;
    show_current_config();
    test_logging();
    
    // �ȴ�һ�£�ȷ�������ļ�ʱ��������仯
    sleep(1);
    
    // 2. ����ʱ�޸ģ�ֻ���ùؼ�����
    cout << "\n�׶�2: �޸������ļ���ֻ���ùؼ�����..." << endl;
    modify_config_file(7);   // 7 = 1+2+4 (FATAL+WARNING+NOTICE)
    
    // �ȴ��������أ���־ϵͳ�����´δ���ʱ��������ļ���
    cout << "�ȴ���������..." << endl;
    LOG_NOTICE("�������ü�����־");  // ��ᴥ�����ü��
    sleep(2);  // ��һ��ʱ������������
    
    cout << "\n�������غ��״̬��" << endl;
    show_current_config();
    test_logging();
    
    // �ȴ�һ��
    sleep(1);
    
    // 3. �ٴ��޸ģ�ֻ����DEBUG����
    cout << "\n�׶�3: �ٴ��޸����ã�ֻ����DEBUG����..." << endl;
    modify_config_file(16);  // 16 = ֻ��DEBUG����
    
    cout << "�ȴ���������..." << endl;
    LOG_DEBUG("�������ü���DEBUG��־");
    sleep(2);
    
    cout << "\n�ٴ����غ��״̬��" << endl;
    show_current_config();
    test_logging();
    
    // 4. ���ղ��ԣ��ָ�ȫ������
    cout << "\n�׶�4: �ָ�ȫ������..." << endl;
    modify_config_file(31);
    
    LOG_NOTICE("���ղ��Դ���");
    sleep(2);
    
    cout << "\n����״̬��" << endl;
    show_current_config();
    test_logging();
    
    cout << "\n=== ����ʱ�����޸Ĳ������ ===" << endl;
    cout << "��� logs/ Ŀ¼�鿴��ͬ�׶����ɵ���־�ļ�" << endl;
    
    return 0;
} 