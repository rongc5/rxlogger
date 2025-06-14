CXX = g++
CXXFLAGS = -std=c++11 -Wall -g -O2 -pthread
INCLUDES = -I./include
SRCDIR = src
TESTDIR = test
OBJDIR = obj

# 源文件
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

# 基本测试文件
TEST_SOURCES = $(TESTDIR)/test_logger.cpp
TEST_OBJECTS = $(TEST_SOURCES:$(TESTDIR)/%.cpp=$(OBJDIR)/%.o)

# 目标
TARGET = test_logger
COMPREHENSIVE_TARGET = comprehensive_test
LIBRARY = liblogger.a

.PHONY: all clean test comprehensive install

all: $(LIBRARY) $(TARGET)

comprehensive: $(LIBRARY) $(COMPREHENSIVE_TARGET)

# 创建对象文件目录
$(OBJDIR):
	mkdir -p $(OBJDIR)

# 编译源文件
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# 编译测试文件
$(OBJDIR)/%.o: $(TESTDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# 创建静态库
$(LIBRARY): $(OBJECTS)
	ar rcs $@ $^

# 创建测试程序
$(TARGET): $(TEST_OBJECTS) $(LIBRARY)
	$(CXX) $(CXXFLAGS) $^ -o $@ -pthread

# 创建全面测试程序
$(COMPREHENSIVE_TARGET): $(OBJDIR)/comprehensive_test.o $(LIBRARY)
	$(CXX) $(CXXFLAGS) $^ -o $@ -pthread

# 运行测试
test: $(TARGET)
	cd $(TESTDIR) && ../$(TARGET)

# 运行全面测试
comprehensive-test: $(COMPREHENSIVE_TARGET)
	cd $(TESTDIR) && ../$(COMPREHENSIVE_TARGET)

# 清理
clean:
	rm -rf $(OBJDIR) $(LIBRARY) $(TARGET) $(COMPREHENSIVE_TARGET)
	rm -rf $(TESTDIR)/logs $(TESTDIR)/log_conf_dump $(TESTDIR)/test_custom.log

# 安装头文件到系统
install:
	sudo cp -r include/. /usr/local/include/logger/
	sudo cp $(LIBRARY) /usr/local/lib/

# 卸载
uninstall:
	sudo rm -rf /usr/local/include/logger
	sudo rm -f /usr/local/lib/$(LIBRARY) 