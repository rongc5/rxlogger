# RxLogger ��־ϵͳ

�� myframe/core �г�ȡ�Ķ�����־ϵͳ��֧�ֶ༶����־���첽д�롢�ļ���ת�ȹ��ܡ�

## ��������

- **�༶����־**: FATAL��WARNING��NOTICE��TRACE��DEBUG
- **�첽д��**: ֧���첽�߳�д�룬���������߳�
- **�ļ���ת**: ����־�ļ�����ָ����Сʱ�Զ���ת
- **����������**: ֧�������ļ�������
- **��ʽ�ӿ�**: ֧�� C++ ��ʽ��־��¼
- **�Զ����ļ�**: ֧��д��ָ���ļ�

## Ŀ¼�ṹ

```
rxlogger/
������ docs/             # �ĵ�Ŀ¼
��   ������ QUICK_START.md       # ��������ָ��
��   ������ USAGE_GUIDE.md       # ʹ��ָ��
��   ������ PROJECT_SUMMARY.md   # ��Ŀ�ܽ�
������ example/          # ʾ������
��   ������ simple_example.cpp
������ include/          # ͷ�ļ�
��   ������ logger_def.h     # ��������
��   ������ logger_util.h    # ���ߺ���
��   ������ log_base.h       # ��־���û���
��   ������ base_thread.h    # �̻߳���͵���
��   ������ log_thread.h     # ��־�߳���
��   ������ log_helper.h     # ��ݺ궨��
������ src/              # Դ�ļ�
��   ������ logger_util.cpp
��   ������ log_base.cpp
��   ������ base_thread.cpp
��   ������ log_thread.cpp
������ test/             # ���Գ���
��   ������ test_logger.cpp
��   ������ comprehensive_test.cpp
��   ������ logger.conf
������ obj/              # ��������ļ�
������ liblogger.a       # ��̬���ļ�
������ Makefile
������ README.md
```

## ���밲װ

```bash
# ����
make

# ���в���
make test

# ��װ��ϵͳ
make install

# ����
make clean
```

## ʹ�÷���

### 1. ��ʼ����־ϵͳ

```cpp
#include "log_helper.h"

int main() {
    // ��ʼ����־ϵͳ��ָ�������ļ�·��
    LOG_INIT("logger.conf");
    
    // ���ҵ�����...
    
    return 0;
}
```

### 2. �����ļ���ʽ

���� `logger.conf` �ļ���

```ini
# �ļ�����С (�ֽ�)
file_max_size=10485760

# ��־·��
log_path=logs

# ��־�ļ�ǰ׺��
prefix_file_name=myapp

# ��־����: 1=FATAL, 2=WARNING, 4=NOTICE, 8=TRACE, 16=DEBUG
# ����ͨ��λ������ϣ��� 31 = 1+2+4+8+16 ��ʾ���м���
type=31

# ��־ģʽ: 1=����ͬ��, 2=�߳��첽
model=2
```

### 3. ʹ����־

```cpp
// ��ʽ����־
LOG_DEBUG("������Ϣ����ֵ: %d", 42);
LOG_TRACE("������Ϣ���ַ���: %s", "����");
LOG_NOTICE("֪ͨ��Ϣ");
LOG_WARNING("������Ϣ��������: %d", -1);
LOG_FATAL("����������Ϣ");

// ��ʽ��־
LOGDEBUG << "������Ϣ: " << 123 << " �ַ���: " << "����";
LOGTRACE << "������Ϣ: " << 3.14;
LOGNOTICE << "֪ͨ��Ϣ";
LOGWARNING << "������Ϣ";
LOGFATAL << "����������Ϣ";

// д���Զ����ļ�
FILE_WRITE("custom.log", "�Զ����ļ���־: %s", "����");
```

### 4. ��־�ļ�

��־����ݼ���ֱ�д�벻ͬ�ļ���
- `myapp.ft` - FATAL ������־
- `myapp.wn` - WARNING ������־
- `myapp.nt` - NOTICE ������־
- `myapp.tc` - TRACE ������־
- `myapp.db` - DEBUG ������־

���ļ���С�������õ�����ʱ�����Զ�������Ϊ `filename.YYYYMMDDHHMMSS` ��ʽ��

## ��־��ʽ

```
[log_id:1000][myapp]:[2023-12-07 14:30:25.123] DEBUG:[140123456789]:[42:main:test.cpp] ���ǵ�����Ϣ
```

��ʽ˵����
- `log_id`: ��־���к�
- `myapp`: ��������
- `2023-12-07 14:30:25.123`: ʱ�������ȷ�����룩
- `DEBUG`: ��־����
- `140123456789`: �߳�ID
- `42:main:test.cpp`: �к�:������:�ļ���
- �������־����

## �̰߳�ȫ

��־ϵͳ���̰߳�ȫ�ģ�֧�ֶ��̲߳���д�롣

## ���ܿ���

- ʹ���첽ģʽ (`model=2`) ���Ա�������ҵ���߳�
- ��־�������ͨ�����ö�̬���������ٲ���Ҫ����־���
- ֧�����������أ������������򼴿ɵ�����־����

## �����ĵ�

��ϸ�ĵ���ο� `docs/` Ŀ¼��

- **[��������](docs/QUICK_START.md)** - 5��������ָ��
- **[ʹ��ָ��](docs/USAGE_GUIDE.md)** - ��ϸ��API�����ú�����ʱ�޸�˵��
- **[��Ŀ�ܽ�](docs/PROJECT_SUMMARY.md)** - ��Ŀ�����ܽ������

## ��Ŀ״̬

? **����ɲ�����֤����������������**

- ����ɹ������ɾ�̬�� `liblogger.a`
- ���к��Ĺ�����֤ͨ��
- ���ܲ��Ա������㣨31����/�룩
- ֧������������
- ���̰߳�ȫ 

## Benchmark

Quick start for performance benchmarking:

```
# Build all targets via CMake
./scripts/build_cmake.sh

# Run benchmark: [total_logs] [threads]
./scripts/run_bench.sh 10000 4
```

See detailed scenarios, parameters and metrics in `docs/BENCHMARK.md`.
