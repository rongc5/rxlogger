# Logger日志库快速入门指南

## 5分钟集成指南

### 1. 编译安装
```bash
git clone <your-logger-repo>
cd logger
make all
sudo make install  # 安装到系统目录
```

### 2. 创建项目
```cpp
// main.cpp
#include "log_helper.h"

int main() {
    // 初始化日志系统
    LOG_INIT("logger.conf");
    
    // 使用日志
    LOG_DEBUG("调试信息: %d", 42);
    LOG_NOTICE("通知信息");
    LOG_WARNING("警告信息");
    LOG_FATAL("错误信息");
    
    // 流式日志
    LOGDEBUG << "流式日志: " << 123;
    
    return 0;
}
```

### 3. 配置文件
```ini
# logger.conf
file_max_size=10485760
log_path=logs
prefix_file_name=myapp
type=31     # 启用所有级别
model=2     # 异步模式
```

### 4. 编译运行
```bash
g++ -std=c++11 main.cpp -llogger -lpthread -o myapp
./myapp
```

## 核心特性

- ? **高性能**: 3.2微秒/条，31万条/秒
- ? **线程安全**: 支持多线程并发
- ? **5种日志级别**: DEBUG, TRACE, NOTICE, WARNING, FATAL
- ? **异步写入**: 不阻塞业务线程
- ? **自动轮转**: 文件大小控制
- ? **零依赖**: 仅需C++11和pthread

## 性能对比

| 日志库 | 性能(条/秒) | 内存使用 | 多线程 |
|-------|------------|----------|--------|
| **本库** | **310,000** | 低 | ? |
| spdlog | 400,000 | 中 | ? |
| glog | 100,000 | 高 | ? |

## 使用建议

### 生产环境配置
```ini
type=7      # 只记录FATAL+WARNING+NOTICE
model=2     # 异步模式
file_max_size=52428800  # 50MB
```

### 性能优化技巧
```cpp
// ? 条件日志，避免高频写入
if (counter % 1000 == 0) {
    LOG_DEBUG("进度: %d", counter);
}

// ? 使用流式日志减少格式化开销
LOGDEBUG << "数据: " << data;

// ? 异步模式，业务线程不阻塞
// logger.conf: model=2
```

## 常见用法

### 错误处理
```cpp
try {
    // 业务逻辑
} catch (const std::exception& e) {
    LOG_FATAL("异常: %s", e.what());
}
```

### 性能监控
```cpp
auto start = std::chrono::high_resolution_clock::now();
// ... 业务逻辑 ...
auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
    std::chrono::high_resolution_clock::now() - start);
LOG_NOTICE("操作耗时: %ld ms", duration.count());
```

### 多线程日志
```cpp
std::vector<std::thread> threads;
for (int i = 0; i < 10; ++i) {
    threads.emplace_back([i]() {
        LOG_DEBUG("线程 %d 开始工作", i);
        // 工作逻辑...
        LOG_DEBUG("线程 %d 完成", i);
    });
}
```

## 输出示例

```
[log_id:1000][myapp]:[2023-12-07 14:30:25.123] DEBUG:[140123456789]:[42:main:main.cpp] 调试信息: 42
[log_id:1001][myapp]:[2023-12-07 14:30:25.124] NOTICE:[140123456789]:[43:main:main.cpp] 通知信息
[log_id:1002][myapp]:[2023-12-07 14:30:25.125] WARNING:[140123456789]:[44:main:main.cpp] 警告信息
```

## 故障排除

### 编译错误
```bash
# 确保C++11支持
g++ -std=c++11 ...

# 确保链接pthread
... -lpthread

# 检查头文件路径
... -I/usr/local/include/logger
```

### 日志文件未生成
1. 检查目录权限: `mkdir -p logs && chmod 755 logs`
2. 检查配置文件路径
3. 验证日志级别设置

## 项目集成示例

详细示例请参考：
- `USAGE_GUIDE.md` - 完整使用指南
- `PERFORMANCE_ANALYSIS.md` - 性能分析报告  
- `example/simple_example.cpp` - 完整示例程序

---

**推荐指数: ?????**  
适合高性能C++项目，特别是服务器应用和实时系统。 