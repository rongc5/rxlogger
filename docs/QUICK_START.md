# Logger��־���������ָ��

## 5���Ӽ���ָ��

### 1. ���밲װ
```bash
git clone <your-logger-repo>
cd logger
make all
sudo make install  # ��װ��ϵͳĿ¼
```

### 2. ������Ŀ
```cpp
// main.cpp
#include "log_helper.h"

int main() {
    // ��ʼ����־ϵͳ
    LOG_INIT("logger.conf");
    
    // ʹ����־
    LOG_DEBUG("������Ϣ: %d", 42);
    LOG_NOTICE("֪ͨ��Ϣ");
    LOG_WARNING("������Ϣ");
    LOG_FATAL("������Ϣ");
    
    // ��ʽ��־
    LOGDEBUG << "��ʽ��־: " << 123;
    
    return 0;
}
```

### 3. �����ļ�
```ini
# logger.conf
file_max_size=10485760
log_path=logs
prefix_file_name=myapp
type=31     # �������м���
model=2     # �첽ģʽ
```

### 4. ��������
```bash
g++ -std=c++11 main.cpp -llogger -lpthread -o myapp
./myapp
```

## ��������

- ? **������**: 3.2΢��/����31����/��
- ? **�̰߳�ȫ**: ֧�ֶ��̲߳���
- ? **5����־����**: DEBUG, TRACE, NOTICE, WARNING, FATAL
- ? **�첽д��**: ������ҵ���߳�
- ? **�Զ���ת**: �ļ���С����
- ? **������**: ����C++11��pthread

## ���ܶԱ�

| ��־�� | ����(��/��) | �ڴ�ʹ�� | ���߳� |
|-------|------------|----------|--------|
| **����** | **310,000** | �� | ? |
| spdlog | 400,000 | �� | ? |
| glog | 100,000 | �� | ? |

## ʹ�ý���

### ������������
```ini
type=7      # ֻ��¼FATAL+WARNING+NOTICE
model=2     # �첽ģʽ
file_max_size=52428800  # 50MB
```

### �����Ż�����
```cpp
// ? ������־�������Ƶд��
if (counter % 1000 == 0) {
    LOG_DEBUG("����: %d", counter);
}

// ? ʹ����ʽ��־���ٸ�ʽ������
LOGDEBUG << "����: " << data;

// ? �첽ģʽ��ҵ���̲߳�����
// logger.conf: model=2
```

## �����÷�

### ������
```cpp
try {
    // ҵ���߼�
} catch (const std::exception& e) {
    LOG_FATAL("�쳣: %s", e.what());
}
```

### ���ܼ��
```cpp
auto start = std::chrono::high_resolution_clock::now();
// ... ҵ���߼� ...
auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
    std::chrono::high_resolution_clock::now() - start);
LOG_NOTICE("������ʱ: %ld ms", duration.count());
```

### ���߳���־
```cpp
std::vector<std::thread> threads;
for (int i = 0; i < 10; ++i) {
    threads.emplace_back([i]() {
        LOG_DEBUG("�߳� %d ��ʼ����", i);
        // �����߼�...
        LOG_DEBUG("�߳� %d ���", i);
    });
}
```

## ���ʾ��

```
[log_id:1000][myapp]:[2023-12-07 14:30:25.123] DEBUG:[140123456789]:[42:main:main.cpp] ������Ϣ: 42
[log_id:1001][myapp]:[2023-12-07 14:30:25.124] NOTICE:[140123456789]:[43:main:main.cpp] ֪ͨ��Ϣ
[log_id:1002][myapp]:[2023-12-07 14:30:25.125] WARNING:[140123456789]:[44:main:main.cpp] ������Ϣ
```

## �����ų�

### �������
```bash
# ȷ��C++11֧��
g++ -std=c++11 ...

# ȷ������pthread
... -lpthread

# ���ͷ�ļ�·��
... -I/usr/local/include/logger
```

### ��־�ļ�δ����
1. ���Ŀ¼Ȩ��: `mkdir -p logs && chmod 755 logs`
2. ��������ļ�·��
3. ��֤��־��������

## ��Ŀ����ʾ��

��ϸʾ����ο���
- `USAGE_GUIDE.md` - ����ʹ��ָ��
- `PERFORMANCE_ANALYSIS.md` - ���ܷ�������  
- `example/simple_example.cpp` - ����ʾ������

---

**�Ƽ�ָ��: ?????**  
�ʺϸ�����C++��Ŀ���ر��Ƿ�����Ӧ�ú�ʵʱϵͳ�� 