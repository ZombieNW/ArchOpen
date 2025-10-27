CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -O2
CPPFLAGS = -I./include

TARGET = ArchOpen
TARGET_DIR = dist
TARGET_NAME = $(TARGET).exe

SRCS = src/main.cpp \
       src/commands.cpp \
       src/configmanager.cpp \
       src/configmigrator.cpp \
       src/romlauncher.cpp

OBJS = $(SRCS:.cpp=.o)

ifeq ($(OS),Windows_NT)
	RM = del /f /q
else
	RM = rm -f
endif

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET_DIR)/$(TARGET_NAME)
	@echo Build complete: $(TARGET_DIR)/$(TARGET_NAME)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS)

test: $(TARGET)
	./$(TARGET_DIR)/$(TARGET_NAME) testfile.nes

