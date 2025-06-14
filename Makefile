CXX = g++
CXXFLAGS = -std=c++11 -Wall -g -O2 -pthread
INCLUDES = -I./include
SRCDIR = src
TESTDIR = test
OBJDIR = obj

# Դ�ļ�
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

# ���������ļ�
TEST_SOURCES = $(TESTDIR)/test_logger.cpp
TEST_OBJECTS = $(TEST_SOURCES:$(TESTDIR)/%.cpp=$(OBJDIR)/%.o)

# Ŀ��
TARGET = test_logger
COMPREHENSIVE_TARGET = comprehensive_test
LIBRARY = liblogger.a

.PHONY: all clean test comprehensive install

all: $(LIBRARY) $(TARGET)

comprehensive: $(LIBRARY) $(COMPREHENSIVE_TARGET)

# ���������ļ�Ŀ¼
$(OBJDIR):
	mkdir -p $(OBJDIR)

# ����Դ�ļ�
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# ��������ļ�
$(OBJDIR)/%.o: $(TESTDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# ������̬��
$(LIBRARY): $(OBJECTS)
	ar rcs $@ $^

# �������Գ���
$(TARGET): $(TEST_OBJECTS) $(LIBRARY)
	$(CXX) $(CXXFLAGS) $^ -o $@ -pthread

# ����ȫ����Գ���
$(COMPREHENSIVE_TARGET): $(OBJDIR)/comprehensive_test.o $(LIBRARY)
	$(CXX) $(CXXFLAGS) $^ -o $@ -pthread

# ���в���
test: $(TARGET)
	cd $(TESTDIR) && ../$(TARGET)

# ����ȫ�����
comprehensive-test: $(COMPREHENSIVE_TARGET)
	cd $(TESTDIR) && ../$(COMPREHENSIVE_TARGET)

# ����
clean:
	rm -rf $(OBJDIR) $(LIBRARY) $(TARGET) $(COMPREHENSIVE_TARGET)
	rm -rf $(TESTDIR)/logs $(TESTDIR)/log_conf_dump $(TESTDIR)/test_custom.log

# ��װͷ�ļ���ϵͳ
install:
	sudo cp -r include/. /usr/local/include/logger/
	sudo cp $(LIBRARY) /usr/local/lib/

# ж��
uninstall:
	sudo rm -rf /usr/local/include/logger
	sudo rm -f /usr/local/lib/$(LIBRARY) 