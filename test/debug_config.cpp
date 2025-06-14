#include "../include/log_helper.h"
#include "../include/log_base.h"
#include <iostream>

using namespace std;

int main() {
    cout << "=== ��־���õ��Թ��� ===" << endl;
    
    // ��ʼ����־ϵͳ
    LOG_INIT("logger.conf");
    
    // ��ȡ��־ϵͳʵ��
    log_thread* thread = base_singleton<log_thread>::get_instance_ex();
    if (!thread) {
        cout << "�����޷���ȡ��־�߳�ʵ��" << endl;
        return 1;
    }
    
    // ��������־����
    cout << "�����־����֧��״̬��" << endl;
    cout << "FATAL   (" << LOGTYPEFATAL   << "): " << (thread->check_type(LOGTYPEFATAL)   ? "����" : "����") << endl;
    cout << "WARNING (" << LOGTYPEWARNING << "): " << (thread->check_type(LOGTYPEWARNING) ? "����" : "����") << endl;
    cout << "NOTICE  (" << LOGTYPENOTICE  << "): " << (thread->check_type(LOGTYPENOTICE)  ? "����" : "����") << endl;
    cout << "TRACE   (" << LOGTYPETRACE   << "): " << (thread->check_type(LOGTYPETRACE)   ? "����" : "����") << endl;
    cout << "DEBUG   (" << LOGTYPEDEBUG   << "): " << (thread->check_type(LOGTYPEDEBUG)   ? "����" : "����") << endl;
    
    // ����WARNING��־д��
    cout << "\nд��WARNING��־����..." << endl;
    LOG_WARNING("������WARNING��־");
    
    sleep(2);
    cout << "�������" << endl;
    
    return 0;
} 