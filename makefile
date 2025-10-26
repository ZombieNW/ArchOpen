TARGET_DIR = dist
TARGET_NAME = ArchOpen

main: src/main.cpp
	@if not exist "$(TARGET_DIR)" mkdir "$(TARGET_DIR)"
	g++ -o "$(TARGET_DIR)/$(TARGET_NAME)" src/main.cpp src/commands.cpp

test:
	./$(TARGET_DIR)/$(TARGET_NAME).exe ./$(TARGET_DIR)/testfile.nes

