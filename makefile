CXX = g++
CXXFLAGS = -Wall -g -std=c++17

CPPFLAGS = -I./include

TARGET = ArchOpen
TARGET_DIR = dist
TARGET_NAME = $(TARGET).exe

SRCS = src/main.cpp src/commands.cpp src/configmanager.cpp src/configmigrator.cpp src/romlauncher.cpp
OBJS = $(SRCS:.cpp=.o)


all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET_DIR)/$(TARGET_NAME)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

clean:
	del /s /q *.o *.exe

test:
	./$(TARGET_DIR)/$(TARGET_NAME) testfile.nes

