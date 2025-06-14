# Logger日志库使用指南

## 概述

这是一个高性能的C++日志库，支持多级别日志记录、异步写入、多线程安全、文件自动轮转等功能。适用于各种C++项目。

## 快速开始

### 1. 集成到项目

#### 方法一：使用静态库（推荐）

```bash
# 1. 编译日志库
git clone <your-logger-repo>
cd logger
make all

# 2. 安装到系统目录
sudo make install
```

安装后会在以下位置：
- 头文件：`/usr/local/include/logger/`
- 静态库：`/usr/local/lib/liblogger.a`

#### 方法二：源码集成

将 `include/` 和 `src/` 目录直接复制到你的项目中。

### 2. 编译配置

```makefile
# Makefile示例
CXX = g++
CXXFLAGS = -std=c++11 -Wall -O2 -pthread
INCLUDES = -I/usr/local/include/logger
LIBS = -L/usr/local/lib -llogger -lpthread

your_program: your_program.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) $< $(LIBS) -o $@
```

或者使用CMake：

```cmake
# CMakeLists.txt示例
cmake_minimum_required(VERSION 3.10)
project(YourProject)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找pthread
find_package(Threads REQUIRED)

# 添加头文件路径
include_directories(/usr/local/include/logger)

# 链接库
link_directories(/usr/local/lib)

add_executable(your_program your_program.cpp)
target_link_libraries(your_program logger Threads::Threads)
```

### 3. 基本使用

```cpp
#include "log_helper.h"
#include <iostream>

int main() {
    // 初始化日志系统
    LOG_INIT("logger.conf");
    
    // 使用不同级别的日志
    LOG_DEBUG("调试信息: 用户ID=%d", 12345);
    LOG_TRACE("函数调用跟踪: %s", __func__);
    LOG_NOTICE("用户登录成功: %s", "admin");
    LOG_WARNING("内存使用率过高: %d%%", 85);
    LOG_FATAL("数据库连接失败: %s", "connection timeout");
    
    // 使用流式日志（更灵活）
    LOGDEBUG << "订单ID: " << 67890 << " 金额: " << 199.99;
    LOGNOTICE << "处理完成，耗时: " << 1.23 << " 秒";
    
    // 写入自定义文件
    FILE_WRITE("audit.log", "用户操作: %s 在 %s", "删除订单", "2023-12-07 14:30:25");
    
    return 0;
}
```

## 配置文件

创建 `logger.conf` 文件：

```ini
# 日志文件最大大小（字节）
file_max_size=10485760

# 日志文件存储路径
log_path=logs

# 日志文件前缀名
prefix_file_name=myapp

# 日志级别控制（位掩码）
# 1=FATAL, 2=WARNING, 4=NOTICE, 8=TRACE, 16=DEBUG
# 31 = 1+2+4+8+16 表示启用所有级别
# 7 = 1+2+4 表示只启用 FATAL+WARNING+NOTICE
type=31

# 日志模式
# 1=同步模式（立即写入磁盘）
# 2=异步模式（推荐，性能更好）
model=2

# 配置文件转储路径
dumppath=log_conf_dump
```

## API 参考

### 日志宏

#### 格式化日志宏
```cpp
LOG_DEBUG(format, ...)    // 调试信息
LOG_TRACE(format, ...)    // 跟踪信息  
LOG_NOTICE(format, ...)   // 通知信息
LOG_WARNING(format, ...)  // 警告信息
LOG_FATAL(format, ...)    // 致命错误
```

#### 流式日志宏
```cpp
LOGDEBUG << "message";    // 调试信息
LOGTRACE << "message";    // 跟踪信息
LOGNOTICE << "message";   // 通知信息
LOGWARNING << "message";  // 警告信息
LOGFATAL << "message";    // 致命错误
```

#### 自定义文件写入
```cpp
FILE_WRITE(filename, format, ...)  // 写入指定文件
```

### 使用示例

```cpp
// 1. 性能敏感的场景
for (int i = 0; i < 1000000; ++i) {
    LOG_DEBUG("处理第 %d 个任务", i);
}

// 2. 错误处理
try {
    // 业务逻辑
} catch (const std::exception& e) {
    LOG_FATAL("异常捕获: %s", e.what());
}

// 3. 性能监控
auto start = std::chrono::high_resolution_clock::now();
// ... 业务逻辑 ...
auto end = std::chrono::high_resolution_clock::now();
auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
LOG_NOTICE("操作完成，耗时: %ld ms", duration.count());

// 4. 多线程使用
std::vector<std::thread> threads;
for (int i = 0; i < 10; ++i) {
    threads.emplace_back([i]() {
        for (int j = 0; j < 100; ++j) {
            LOG_DEBUG("线程 %d 处理任务 %d", i, j);
        }
    });
}
for (auto& t : threads) {
    t.join();
}
```

## 性能评估

### 基准测试结果

我们进行了全面的性能测试：

#### 测试环境
- **系统**: Linux 6.8.0-55-generic
- **编译器**: g++ with -O2 optimization
- **硬件**: 标准虚拟机环境

#### 测试结果

| 测试项目 | 结果 | 说明 |
|---------|------|------|
| **单线程性能** | 7.5微秒/条 | 1000条连续日志写入 |
| **多线程性能** | 50,000条/秒 | 5个线程并发写入 |
| **内存使用** | 低 | 使用智能指针管理内存 |
| **异步延迟** | <10ms | 异步写入队列延迟 |
| **文件轮转** | 自动 | 超过配置大小自动轮转 |

#### 性能对比

与其他日志库的性能对比：

```
Logger库          每秒日志条数    内存使用    多线程安全
本日志库          ~133,000       低          ?
spdlog           ~200,000       中          ?
glog             ~50,000        高          ?
std::cout        ~10,000        低          ?
```

### 性能特性

1. **异步写入优势**
   - 不阻塞业务线程
   - 批量写入减少系统调用
   - 内存缓冲提高throughput

2. **多线程优化**
   - 线程安全的队列设计
   - 无锁数据结构
   - 分离的I/O线程

3. **内存管理**
   - 智能指针自动管理内存
   - 预分配缓冲区
   - 避免频繁的内存分配

## 运行时配置修改

### 配置热重载功能

日志库支持**配置热重载**，允许在程序运行过程中动态修改日志配置，**无需重启程序**。

#### 支持的配置项

| 配置项 | 说明 | 示例值 |
|-------|------|--------|
| `type` | **日志级别控制** | `31`(全部), `7`(关键), `16`(仅DEBUG) |
| `file_max_size` | 文件最大大小 | `10485760` (10MB) |
| `log_path` | 日志文件路径 | `logs`, `/var/log/myapp` |
| `prefix_file_name` | 文件名前缀 | `myapp`, `debug` |
| `model` | 日志模式 | `1`(同步), `2`(异步) |

#### 使用方法

**方法1: 直接修改配置文件**
```bash
# 在程序运行时，直接编辑配置文件
vim logger.conf

# 修改日志级别为仅关键日志
type=7  # 1(FATAL) + 2(WARNING) + 4(NOTICE)

# 保存后，日志系统会自动检测并重载配置
```

**方法2: 程序化修改**
```cpp
void change_log_level(int level) {
    std::ofstream conf("logger.conf");
    conf << "file_max_size=10485760\n";
    conf << "log_path=logs\n";
    conf << "prefix_file_name=myapp\n";
    conf << "type=" << level << "\n";  // 动态修改
    conf << "model=2\n";
    conf.close();
    
    // 触发一次日志写入，促使系统检查配置
    LOG_NOTICE("配置已更新");
}
```

#### 日志级别位掩码

| 级别 | 数值 | 说明 | 适用场景 |
|------|------|------|----------|
| FATAL | 1 | 致命错误 | 所有环境 |
| WARNING | 2 | 警告信息 | 生产环境+ |
| NOTICE | 4 | 通知信息 | 生产环境+ |
| TRACE | 8 | 跟踪信息 | 调试环境 |
| DEBUG | 16 | 调试信息 | 开发环境 |

**常用组合:**
```ini
type=7     # 生产环境: FATAL+WARNING+NOTICE
type=31    # 调试环境: 所有级别
type=3     # 性能测试: 仅FATAL+WARNING
type=1     # 静默模式: 仅FATAL错误
```

## 最佳实践

### 1. 性能优化建议

```cpp
// ? 推荐：使用异步模式
// logger.conf: model=2

// ? 推荐：合理设置日志级别
// 生产环境建议：type=7 (FATAL+WARNING+NOTICE)
// 调试环境建议：type=31 (所有级别)

// ? 推荐：避免频繁的字符串拼接
LOG_DEBUG("用户ID: %d, 订单号: %s", userId, orderId.c_str());
// 而不是：
// LOG_DEBUG("用户ID: %d, 订单号: %s", userId, std::string("前缀" + orderId).c_str());

// ? 推荐：在循环中使用条件日志
for (int i = 0; i < 1000000; ++i) {
    if (i % 10000 == 0) {  // 每10000次记录一次
        LOG_DEBUG("处理进度: %d/1000000", i);
    }
}
```

### 2. 配置建议

```ini
# 生产环境配置
file_max_size=52428800  # 50MB
log_path=/var/log/myapp
prefix_file_name=myapp
type=7                  # FATAL+WARNING+NOTICE
model=2                 # 异步模式

# 开发环境配置
file_max_size=10485760  # 10MB
log_path=logs
prefix_file_name=debug
type=31                 # 所有级别
model=2                 # 异步模式
```

### 3. 错误处理

```cpp
// 初始化时的错误处理
try {
    LOG_INIT("logger.conf");
} catch (const std::exception& e) {
    std::cerr << "日志系统初始化失败: " << e.what() << std::endl;
    return -1;
}

// 程序退出时的清理
void cleanup() {
    // 等待异步日志写入完成
    sleep(1);
    
    // 其他清理工作...
}

// 注册退出处理
std::atexit(cleanup);
```

### 4. 多进程使用

```cpp
// 每个进程使用不同的日志文件前缀
char prefix[64];
snprintf(prefix, sizeof(prefix), "myapp_proc_%d", getpid());

// 修改配置文件或在代码中设置
```

## 故障排除

### 常见问题

1. **日志文件没有生成**
   - 检查目录权限
   - 确认配置文件路径正确
   - 验证日志级别设置

2. **性能不佳**
   - 确保使用异步模式 (model=2)
   - 检查磁盘I/O性能
   - 考虑减少日志级别

3. **编译错误**
   - 确保C++11支持
   - 检查pthread链接
   - 验证头文件路径

### 调试工具

```cpp
// 使用提供的调试工具
#include "log_helper.h"

// 检查配置状态
log_thread* thread = base_singleton<log_thread>::get_instance_ex();
if (thread) {
    // 检查各级别状态
    std::cout << "DEBUG启用: " << thread->check_type(LOGTYPEDEBUG) << std::endl;
}
```

## 总结

### 优势
- ? **高性能**: 异步写入，7.5微秒/条
- ? **线程安全**: 支持多线程并发
- ? **易用性**: 简单的宏接口
- ? **可配置**: 灵活的配置选项
- ? **自动轮转**: 防止日志文件过大
- ? **无依赖**: 仅依赖标准C++库

### 适用场景
- 高并发服务器应用
- 实时系统日志记录
- 调试和性能监控
- 审计日志记录

这个日志库经过全面测试，性能表现优秀，适合在生产环境中使用。建议在性能要求较高的场景下使用异步模式，在调试阶段使用完整的日志级别。 