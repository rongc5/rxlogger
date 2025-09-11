# RxLogger 性能压测指南

本文档介绍如何使用项目内置的压测程序 `perf_bench` 评估不同日志等级、不同线程数下的写入性能与吞吐。

## 一、构建

使用 CMake 构建（默认输出到 `build/bin`，并处理中文编码为 UTF-8）：

```
./scripts/build_cmake.sh
```

构建完成后，可执行文件位于：`build/bin/perf_bench`

## 二、快速运行

仓库已提供便捷脚本：

```
# 语法：run_bench.sh [总日志数] [线程数]
./scripts/run_bench.sh 10000 1
```

示例输出：

```
RxLogger perf bench: count=10000, threads=1
produce time: 14402 us, per log: 1.4402 us, throughput: 694348 logs/s
done. logs under test/logs with prefix perf_bench.*
```

生成的日志文件在 `test/logs/` 目录，前缀为 `perf_bench.*`（包含 db/tc/nt/wn/ft 五个等级）。

## 三、程序参数

`perf_bench` 支持两个可选参数：

- 第一个参数：总日志条数（默认 10000）
- 第二个参数：线程数（默认 1）

例如：

```
# 单线程写 10000 条
./build/bin/perf_bench 10000 1

# 4 线程写 100000 条
./build/bin/perf_bench 100000 4
```

## 四、场景建议

- 单线程短打：`10000` 条，`threads=1`（冷启动、吞吐基线）
- 多线程并发：`100000` 条，`threads=4/8`（CPU 压力与队列吞吐）
- 等级混合：程序内部采用轮询写入五种等级（DEBUG/TRACE/NOTICE/WARNING/FATAL），可覆盖各等级文件写入路径
- 文件系统压力：将 `test/perf_logger.conf` 中 `log_path` 指向高速盘/网络盘，观察差异
- 超大批量：`1000000` 条日志（建议仅在生产规格机器上测试），关注总耗时和落盘稳定性

## 五、指标说明

- produce time(us)：产生日志（提交到异步队列）耗时
- per log(us)：单条平均耗时
- throughput(logs/s)：日志产生吞吐（不含最终磁盘 flush 时间）。程序末尾会等待 500ms，确保异步队列落盘基本完成。

## 六、配置说明

压测使用 `test/perf_logger.conf`：

```
file_max_size=104857600
log_path=logs
prefix_file_name=perf_bench
type=31
model=2
dumppath=log_conf_dump
```

- `type=31` 开启所有等级（1+2+4+8+16）
- `model=2` 采用异步写入模型

如需定向到其他目录或限制文件大小，可自行调整该配置并重跑。

## 七、注意事项

- 终端/环境建议使用 UTF-8（或使用本仓库 CMake 选项已自动处理 GBK→UTF-8 编码）
- 如需对比不同实现（如流式 vs printf），可扩展 `perf_bench` 在相同条件下分别运行两种宏，输出对比指标
- 若需要 CSV/JSON 导出指标，欢迎提交 issue 或 PR，我们也可在程序中增加导出开关

