cmake_minimum_required(VERSION 3.10)
project(RxLogger)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置编译选项
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g -O2 -pthread")

# 包含目录
include_directories(include)

# 收集源文件
file(GLOB LOGGER_SOURCES "src/*.cpp")

# 创建静态库
add_library(logger STATIC ${LOGGER_SOURCES})

# 创建测试可执行文件
add_executable(test_logger test/test_logger.cpp)
target_link_libraries(test_logger logger pthread)

# 创建综合测试可执行文件
add_executable(comprehensive_test test/comprehensive_test.cpp)
target_link_libraries(comprehensive_test logger pthread)

# 设置输出目录
set_target_properties(logger PROPERTIES 
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    OUTPUT_NAME "logger"
)

set_target_properties(test_logger comprehensive_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

# 自定义目标
add_custom_target(all_targets ALL DEPENDS logger test_logger comprehensive_test)

# 测试目标
enable_testing()
add_test(NAME logger_test COMMAND test_logger WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test")
add_test(NAME comprehensive_logger_test COMMAND comprehensive_test WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test")

# 安装规则
install(DIRECTORY include/ DESTINATION include/logger)
install(TARGETS logger DESTINATION lib)