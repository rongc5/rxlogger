## Makefile (out-of-source outputs under build/make)

CXX = g++
CXXFLAGS = -std=c++11 -Wall -g -O2 -pthread
INCLUDES = -I./include
SRCDIR = src
TESTDIR = test

# Out-of-source build dirs
BUILDDIR = build/make
OBJDIR = $(BUILDDIR)/obj
LIBDIR = $(BUILDDIR)/lib
BINDIR = $(BUILDDIR)/bin

# Sources/objects
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

# Test sources for simple build (keep minimal)
TEST_SOURCES = $(TESTDIR)/test_logger.cpp
TEST_OBJECTS = $(TEST_SOURCES:$(TESTDIR)/%.cpp=$(OBJDIR)/%.o)

# Targets
TARGET = $(BINDIR)/test_logger
COMPREHENSIVE_TARGET = $(BINDIR)/comprehensive_test
LIBRARY = $(LIBDIR)/liblogger.a

.PHONY: all clean test comprehensive install uninstall

all: $(LIBRARY) $(TARGET)

comprehensive: $(LIBRARY) $(COMPREHENSIVE_TARGET)

# Ensure build directories exist
$(OBJDIR):
	mkdir -p $(OBJDIR)
	mkdir -p $(LIBDIR)
	mkdir -p $(BINDIR)

# Build object files from src
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Build object files from test
$(OBJDIR)/%.o: $(TESTDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Static library
$(LIBRARY): $(OBJECTS)
	ar rcs $@ $^

# Link test binaries
$(TARGET): $(TEST_OBJECTS) $(LIBRARY)
	$(CXX) $(CXXFLAGS) $^ -o $@ -pthread

$(COMPREHENSIVE_TARGET): $(OBJDIR)/comprehensive_test.o $(LIBRARY)
	$(CXX) $(CXXFLAGS) $^ -o $@ -pthread

# Run tests
test: $(TARGET)
	cd $(TESTDIR) && ../$(BINDIR)/test_logger

comprehensive-test: $(COMPREHENSIVE_TARGET)
	cd $(TESTDIR) && ../$(BINDIR)/comprehensive_test

# Clean outputs
clean:
	rm -rf $(BUILDDIR)
	rm -rf $(TESTDIR)/logs $(TESTDIR)/log_conf_dump $(TESTDIR)/test_custom.log

# Install headers + library
install:
	mkdir -p /usr/local/include/logger/
	cp -r include/. /usr/local/include/logger/
	cp $(LIBRARY) /usr/local/lib/

uninstall:
	rm -rf /usr/local/include/logger
	rm -f /usr/local/lib/$(notdir $(LIBRARY))

