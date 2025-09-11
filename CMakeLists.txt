cmake_minimum_required(VERSION 3.10)
project(RxLogger)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置编译选项
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g -O2 -pthread")

# Option: treat sources as GBK, output UTF-8 (fix Chinese garbled output)
option(RXLOGGER_GBK_SOURCE "Treat sources as GBK and emit UTF-8 strings" OFF)
if(RXLOGGER_GBK_SOURCE)
    if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(-finput-charset=GBK -fexec-charset=UTF-8)
    endif()
endif()

 

 
# 包含目录
include_directories(include)

# 收集源文件
file(GLOB LOGGER_SOURCES "src/*.cpp")

# 统一输出目录到 build/{bin,lib}
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# 创建静态库
add_library(logger STATIC ${LOGGER_SOURCES})

# 创建测试可执行文件
add_executable(test_logger test/test_logger.cpp)
target_link_libraries(test_logger logger pthread)

# 创建综合测试可执行文件
add_executable(comprehensive_test test/comprehensive_test.cpp)
target_link_libraries(comprehensive_test logger pthread)

# 设置输出目录
set_target_properties(logger PROPERTIES OUTPUT_NAME "logger")

# 自定义目标
add_custom_target(all_targets ALL DEPENDS logger test_logger comprehensive_test)

# 其他手动示例/工具（非自动化测试）
if (EXISTS "${CMAKE_SOURCE_DIR}/test/test_both_modes.cpp")
    add_executable(test_both_modes test/test_both_modes.cpp)
    target_link_libraries(test_both_modes logger pthread)
    add_dependencies(all_targets test_both_modes)
endif()
if (EXISTS "${CMAKE_SOURCE_DIR}/test/test_fd_cache.cpp")
    add_executable(test_fd_cache test/test_fd_cache.cpp)
    target_link_libraries(test_fd_cache logger pthread)
    add_dependencies(all_targets test_fd_cache)
endif()
if (EXISTS "${CMAKE_SOURCE_DIR}/test/test_file_deletion.cpp")
    add_executable(test_file_deletion test/test_file_deletion.cpp)
    target_link_libraries(test_file_deletion logger pthread)
    add_dependencies(all_targets test_file_deletion)
endif()

# 额外性能/缓存测试示例
if (EXISTS "${CMAKE_SOURCE_DIR}/test/test_lru_cache.cpp")
    add_executable(test_lru_cache test/test_lru_cache.cpp)
    target_link_libraries(test_lru_cache logger pthread)
    add_dependencies(all_targets test_lru_cache)
endif()
if (EXISTS "${CMAKE_SOURCE_DIR}/test/test_direct_vs_stdio.cpp")
    add_executable(test_direct_vs_stdio test/test_direct_vs_stdio.cpp)
    target_link_libraries(test_direct_vs_stdio logger pthread)
    add_dependencies(all_targets test_direct_vs_stdio)
endif()
if (EXISTS "${CMAKE_SOURCE_DIR}/test/test_rename_optimization.cpp")
    add_executable(test_rename_optimization test/test_rename_optimization.cpp)
    target_link_libraries(test_rename_optimization logger pthread)
    add_dependencies(all_targets test_rename_optimization)
endif()
if (EXISTS "${CMAKE_SOURCE_DIR}/test/test_rename_opt.cpp")
    add_executable(test_rename_opt test/test_rename_opt.cpp)
    target_link_libraries(test_rename_opt logger pthread)
    add_dependencies(all_targets test_rename_opt)
endif()

# 测试目标
enable_testing()
add_test(NAME logger_test COMMAND test_logger WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test")
add_test(NAME comprehensive_logger_test COMMAND comprehensive_test WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test")

# 安装规则
install(DIRECTORY include/ DESTINATION include/logger)
install(TARGETS logger DESTINATION lib)
# Perf benchmark
add_executable(perf_bench test/perf_bench.cpp)
target_link_libraries(perf_bench logger pthread)

# Example program
if (EXISTS "${CMAKE_SOURCE_DIR}/example/simple_example.cpp")
    add_executable(simple_example example/simple_example.cpp)
    target_link_libraries(simple_example logger pthread)
    # Handle GBK-encoded source for this example
    if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(simple_example PRIVATE -finput-charset=GBK -fexec-charset=UTF-8)
    endif()
    add_dependencies(all_targets simple_example)
endif()

# 自动发现 test/*.cpp，补充生成可执行与按规则注册 ctest（位于末尾，避免与手工目标冲突）
file(GLOB RXLOGGER_TEST_SOURCES_PREFIX "${CMAKE_SOURCE_DIR}/test/test_*.cpp")
file(GLOB RXLOGGER_TEST_SOURCES_SUFFIX "${CMAKE_SOURCE_DIR}/test/*_test.cpp")
set(RXLOGGER_ALL_TEST_SOURCES ${RXLOGGER_TEST_SOURCES_PREFIX} ${RXLOGGER_TEST_SOURCES_SUFFIX})
# 手动补充特例名（如不以 test_ 开头的综合用例）
if (EXISTS "${CMAKE_SOURCE_DIR}/test/comprehensive_test.cpp")
    list(APPEND RXLOGGER_ALL_TEST_SOURCES "${CMAKE_SOURCE_DIR}/test/comprehensive_test.cpp")
endif()
# 去重
list(REMOVE_DUPLICATES RXLOGGER_ALL_TEST_SOURCES)

# 过滤掉历史/交互式或不兼容用例
set(RXLOGGER_FILTERED_TEST_SOURCES)
foreach(test_src ${RXLOGGER_ALL_TEST_SOURCES})
    get_filename_component(_test_base "${test_src}" NAME)
    if (_test_base MATCHES "^(runtime_config_test|debug_config|test_warning)\\.cpp$")
        # 跳过
    else()
        list(APPEND RXLOGGER_FILTERED_TEST_SOURCES "${test_src}")
    endif()
endforeach()

foreach(test_src ${RXLOGGER_FILTERED_TEST_SOURCES})
    get_filename_component(test_name "${test_src}" NAME_WE)
    if (TARGET ${test_name})
        continue()
    endif()
    add_executable(${test_name} ${test_src})
    target_link_libraries(${test_name} logger pthread)
    if (TARGET all_targets)
        add_dependencies(all_targets ${test_name})
    endif()

    # 规则：名称包含 "test" 则注册为 ctest；但排除交互/长跑/性能类
    string(TOLOWER "${test_name}" test_name_lc)
    set(SAFE_FOR_CTEST TRUE)
    foreach(ex_pat fd_cache file_deletion perf bench)
        string(FIND "${test_name_lc}" "${ex_pat}" ex_idx)
        if (NOT ex_idx EQUAL -1)
            set(SAFE_FOR_CTEST FALSE)
        endif()
    endforeach()
    string(FIND "${test_name_lc}" "test" has_test_kw)
    if (SAFE_FOR_CTEST AND (NOT has_test_kw EQUAL -1))
        add_test(NAME ${test_name} COMMAND ${test_name} WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test")
    endif()
endforeach()
