#include "../include/log_helper.h"
#include <iostream>
#include <unistd.h>

using namespace std;

int main(int argc, char* argv[]) {
    cout << "��ʼ������־ϵͳ..." << endl;
    
    // ����1: ��ʼ����־ϵͳ
    const char* log_conf_path = "test/logger.conf";
    LOG_INIT(log_conf_path);
    
    cout << "��־ϵͳ��ʼ�����" << endl;
    
    // ����2: ʹ�ò�ͬ�������־
    LOG_DEBUG("����һ��������־����ֵ: %d", 42);
    LOG_TRACE("����һ��������־���ַ���: %s", "����");
    LOG_NOTICE("����һ��֪ͨ��־");
    LOG_WARNING("����һ��������־��������: %d", -1);
    LOG_FATAL("����һ������������־");
    
    // ����3: ʹ����ʽ��־
    LOGDEBUG << "��ʽ������־: " << 123 << " �ַ���: " << "����";
    LOGTRACE << "��ʽ������־: " << 3.14;
    LOGNOTICE << "��ʽ֪ͨ��־";
    LOGWARNING << "��ʽ������־";
    LOGFATAL << "��ʽ����������־";
    
    // ����4: д���ض��ļ�
    FILE_WRITE("test_custom.log", "�Զ����ļ���־: %s", "��������");
    
    cout << "�ȴ���־д�����..." << endl;
    sleep(2);
    
    cout << "��־������ɣ����� logs Ŀ¼�µ���־�ļ�" << endl;
    
    return 0;
} 