#include "../include/log_helper.h"
#include <iostream>
#include <unistd.h>

using namespace std;

int main() {
    cout << "����WARNING��־����..." << endl;
    
    // ��ʼ����־ϵͳ
    LOG_INIT("logger.conf");
    
    // ֻдWARNING��־
    cout << "д��WARNING��־..." << endl;
    LOG_WARNING("����һ��WARNING������־��������: %d", 12345);
    
    cout << "д����ʽWARNING��־..." << endl;
    LOGWARNING << "����һ����ʽWARNING������־: " << 67890;
    
    // �ȴ��첽д��
    cout << "�ȴ�д�����..." << endl;
    sleep(3);
    
    cout << "WARNING��־�������" << endl;
    return 0;
} 