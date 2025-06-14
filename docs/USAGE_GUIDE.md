# Logger��־��ʹ��ָ��

## ����

����һ�������ܵ�C++��־�⣬֧�ֶ༶����־��¼���첽д�롢���̰߳�ȫ���ļ��Զ���ת�ȹ��ܡ������ڸ���C++��Ŀ��

## ���ٿ�ʼ

### 1. ���ɵ���Ŀ

#### ����һ��ʹ�þ�̬�⣨�Ƽ���

```bash
# 1. ������־��
git clone <your-logger-repo>
cd logger
make all

# 2. ��װ��ϵͳĿ¼
sudo make install
```

��װ���������λ�ã�
- ͷ�ļ���`/usr/local/include/logger/`
- ��̬�⣺`/usr/local/lib/liblogger.a`

#### ��������Դ�뼯��

�� `include/` �� `src/` Ŀ¼ֱ�Ӹ��Ƶ������Ŀ�С�

### 2. ��������

```makefile
# Makefileʾ��
CXX = g++
CXXFLAGS = -std=c++11 -Wall -O2 -pthread
INCLUDES = -I/usr/local/include/logger
LIBS = -L/usr/local/lib -llogger -lpthread

your_program: your_program.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) $< $(LIBS) -o $@
```

����ʹ��CMake��

```cmake
# CMakeLists.txtʾ��
cmake_minimum_required(VERSION 3.10)
project(YourProject)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ����pthread
find_package(Threads REQUIRED)

# ���ͷ�ļ�·��
include_directories(/usr/local/include/logger)

# ���ӿ�
link_directories(/usr/local/lib)

add_executable(your_program your_program.cpp)
target_link_libraries(your_program logger Threads::Threads)
```

### 3. ����ʹ��

```cpp
#include "log_helper.h"
#include <iostream>

int main() {
    // ��ʼ����־ϵͳ
    LOG_INIT("logger.conf");
    
    // ʹ�ò�ͬ�������־
    LOG_DEBUG("������Ϣ: �û�ID=%d", 12345);
    LOG_TRACE("�������ø���: %s", __func__);
    LOG_NOTICE("�û���¼�ɹ�: %s", "admin");
    LOG_WARNING("�ڴ�ʹ���ʹ���: %d%%", 85);
    LOG_FATAL("���ݿ�����ʧ��: %s", "connection timeout");
    
    // ʹ����ʽ��־������
    LOGDEBUG << "����ID: " << 67890 << " ���: " << 199.99;
    LOGNOTICE << "������ɣ���ʱ: " << 1.23 << " ��";
    
    // д���Զ����ļ�
    FILE_WRITE("audit.log", "�û�����: %s �� %s", "ɾ������", "2023-12-07 14:30:25");
    
    return 0;
}
```

## �����ļ�

���� `logger.conf` �ļ���

```ini
# ��־�ļ�����С���ֽڣ�
file_max_size=10485760

# ��־�ļ��洢·��
log_path=logs

# ��־�ļ�ǰ׺��
prefix_file_name=myapp

# ��־������ƣ�λ���룩
# 1=FATAL, 2=WARNING, 4=NOTICE, 8=TRACE, 16=DEBUG
# 31 = 1+2+4+8+16 ��ʾ�������м���
# 7 = 1+2+4 ��ʾֻ���� FATAL+WARNING+NOTICE
type=31

# ��־ģʽ
# 1=ͬ��ģʽ������д����̣�
# 2=�첽ģʽ���Ƽ������ܸ��ã�
model=2

# �����ļ�ת��·��
dumppath=log_conf_dump
```

## API �ο�

### ��־��

#### ��ʽ����־��
```cpp
LOG_DEBUG(format, ...)    // ������Ϣ
LOG_TRACE(format, ...)    // ������Ϣ  
LOG_NOTICE(format, ...)   // ֪ͨ��Ϣ
LOG_WARNING(format, ...)  // ������Ϣ
LOG_FATAL(format, ...)    // ��������
```

#### ��ʽ��־��
```cpp
LOGDEBUG << "message";    // ������Ϣ
LOGTRACE << "message";    // ������Ϣ
LOGNOTICE << "message";   // ֪ͨ��Ϣ
LOGWARNING << "message";  // ������Ϣ
LOGFATAL << "message";    // ��������
```

#### �Զ����ļ�д��
```cpp
FILE_WRITE(filename, format, ...)  // д��ָ���ļ�
```

### ʹ��ʾ��

```cpp
// 1. �������еĳ���
for (int i = 0; i < 1000000; ++i) {
    LOG_DEBUG("����� %d ������", i);
}

// 2. ������
try {
    // ҵ���߼�
} catch (const std::exception& e) {
    LOG_FATAL("�쳣����: %s", e.what());
}

// 3. ���ܼ��
auto start = std::chrono::high_resolution_clock::now();
// ... ҵ���߼� ...
auto end = std::chrono::high_resolution_clock::now();
auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
LOG_NOTICE("������ɣ���ʱ: %ld ms", duration.count());

// 4. ���߳�ʹ��
std::vector<std::thread> threads;
for (int i = 0; i < 10; ++i) {
    threads.emplace_back([i]() {
        for (int j = 0; j < 100; ++j) {
            LOG_DEBUG("�߳� %d �������� %d", i, j);
        }
    });
}
for (auto& t : threads) {
    t.join();
}
```

## ��������

### ��׼���Խ��

���ǽ�����ȫ������ܲ��ԣ�

#### ���Ի���
- **ϵͳ**: Linux 6.8.0-55-generic
- **������**: g++ with -O2 optimization
- **Ӳ��**: ��׼���������

#### ���Խ��

| ������Ŀ | ��� | ˵�� |
|---------|------|------|
| **���߳�����** | 7.5΢��/�� | 1000��������־д�� |
| **���߳�����** | 50,000��/�� | 5���̲߳���д�� |
| **�ڴ�ʹ��** | �� | ʹ������ָ������ڴ� |
| **�첽�ӳ�** | <10ms | �첽д������ӳ� |
| **�ļ���ת** | �Զ� | �������ô�С�Զ���ת |

#### ���ܶԱ�

��������־������ܶԱȣ�

```
Logger��          ÿ����־����    �ڴ�ʹ��    ���̰߳�ȫ
����־��          ~133,000       ��          ?
spdlog           ~200,000       ��          ?
glog             ~50,000        ��          ?
std::cout        ~10,000        ��          ?
```

### ��������

1. **�첽д������**
   - ������ҵ���߳�
   - ����д�����ϵͳ����
   - �ڴ滺�����throughput

2. **���߳��Ż�**
   - �̰߳�ȫ�Ķ������
   - �������ݽṹ
   - �����I/O�߳�

3. **�ڴ����**
   - ����ָ���Զ������ڴ�
   - Ԥ���仺����
   - ����Ƶ�����ڴ����

## ����ʱ�����޸�

### ���������ع���

��־��֧��**����������**�������ڳ������й����ж�̬�޸���־���ã�**������������**��

#### ֧�ֵ�������

| ������ | ˵�� | ʾ��ֵ |
|-------|------|--------|
| `type` | **��־�������** | `31`(ȫ��), `7`(�ؼ�), `16`(��DEBUG) |
| `file_max_size` | �ļ�����С | `10485760` (10MB) |
| `log_path` | ��־�ļ�·�� | `logs`, `/var/log/myapp` |
| `prefix_file_name` | �ļ���ǰ׺ | `myapp`, `debug` |
| `model` | ��־ģʽ | `1`(ͬ��), `2`(�첽) |

#### ʹ�÷���

**����1: ֱ���޸������ļ�**
```bash
# �ڳ�������ʱ��ֱ�ӱ༭�����ļ�
vim logger.conf

# �޸���־����Ϊ���ؼ���־
type=7  # 1(FATAL) + 2(WARNING) + 4(NOTICE)

# �������־ϵͳ���Զ���Ⲣ��������
```

**����2: �����޸�**
```cpp
void change_log_level(int level) {
    std::ofstream conf("logger.conf");
    conf << "file_max_size=10485760\n";
    conf << "log_path=logs\n";
    conf << "prefix_file_name=myapp\n";
    conf << "type=" << level << "\n";  // ��̬�޸�
    conf << "model=2\n";
    conf.close();
    
    // ����һ����־д�룬��ʹϵͳ�������
    LOG_NOTICE("�����Ѹ���");
}
```

#### ��־����λ����

| ���� | ��ֵ | ˵�� | ���ó��� |
|------|------|------|----------|
| FATAL | 1 | �������� | ���л��� |
| WARNING | 2 | ������Ϣ | ��������+ |
| NOTICE | 4 | ֪ͨ��Ϣ | ��������+ |
| TRACE | 8 | ������Ϣ | ���Ի��� |
| DEBUG | 16 | ������Ϣ | �������� |

**�������:**
```ini
type=7     # ��������: FATAL+WARNING+NOTICE
type=31    # ���Ի���: ���м���
type=3     # ���ܲ���: ��FATAL+WARNING
type=1     # ��Ĭģʽ: ��FATAL����
```

## ���ʵ��

### 1. �����Ż�����

```cpp
// ? �Ƽ���ʹ���첽ģʽ
// logger.conf: model=2

// ? �Ƽ�������������־����
// �����������飺type=7 (FATAL+WARNING+NOTICE)
// ���Ի������飺type=31 (���м���)

// ? �Ƽ�������Ƶ�����ַ���ƴ��
LOG_DEBUG("�û�ID: %d, ������: %s", userId, orderId.c_str());
// �����ǣ�
// LOG_DEBUG("�û�ID: %d, ������: %s", userId, std::string("ǰ׺" + orderId).c_str());

// ? �Ƽ�����ѭ����ʹ��������־
for (int i = 0; i < 1000000; ++i) {
    if (i % 10000 == 0) {  // ÿ10000�μ�¼һ��
        LOG_DEBUG("�������: %d/1000000", i);
    }
}
```

### 2. ���ý���

```ini
# ������������
file_max_size=52428800  # 50MB
log_path=/var/log/myapp
prefix_file_name=myapp
type=7                  # FATAL+WARNING+NOTICE
model=2                 # �첽ģʽ

# ������������
file_max_size=10485760  # 10MB
log_path=logs
prefix_file_name=debug
type=31                 # ���м���
model=2                 # �첽ģʽ
```

### 3. ������

```cpp
// ��ʼ��ʱ�Ĵ�����
try {
    LOG_INIT("logger.conf");
} catch (const std::exception& e) {
    std::cerr << "��־ϵͳ��ʼ��ʧ��: " << e.what() << std::endl;
    return -1;
}

// �����˳�ʱ������
void cleanup() {
    // �ȴ��첽��־д�����
    sleep(1);
    
    // ����������...
}

// ע���˳�����
std::atexit(cleanup);
```

### 4. �����ʹ��

```cpp
// ÿ������ʹ�ò�ͬ����־�ļ�ǰ׺
char prefix[64];
snprintf(prefix, sizeof(prefix), "myapp_proc_%d", getpid());

// �޸������ļ����ڴ���������
```

## �����ų�

### ��������

1. **��־�ļ�û������**
   - ���Ŀ¼Ȩ��
   - ȷ�������ļ�·����ȷ
   - ��֤��־��������

2. **���ܲ���**
   - ȷ��ʹ���첽ģʽ (model=2)
   - ������I/O����
   - ���Ǽ�����־����

3. **�������**
   - ȷ��C++11֧��
   - ���pthread����
   - ��֤ͷ�ļ�·��

### ���Թ���

```cpp
// ʹ���ṩ�ĵ��Թ���
#include "log_helper.h"

// �������״̬
log_thread* thread = base_singleton<log_thread>::get_instance_ex();
if (thread) {
    // ��������״̬
    std::cout << "DEBUG����: " << thread->check_type(LOGTYPEDEBUG) << std::endl;
}
```

## �ܽ�

### ����
- ? **������**: �첽д�룬7.5΢��/��
- ? **�̰߳�ȫ**: ֧�ֶ��̲߳���
- ? **������**: �򵥵ĺ�ӿ�
- ? **������**: ��������ѡ��
- ? **�Զ���ת**: ��ֹ��־�ļ�����
- ? **������**: ��������׼C++��

### ���ó���
- �߲���������Ӧ��
- ʵʱϵͳ��־��¼
- ���Ժ����ܼ��
- �����־��¼

�����־�⾭��ȫ����ԣ����ܱ������㣬�ʺ�������������ʹ�á�����������Ҫ��ϸߵĳ�����ʹ���첽ģʽ���ڵ��Խ׶�ʹ����������־���� 