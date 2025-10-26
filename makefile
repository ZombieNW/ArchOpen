main: src/main.cpp
	g++ -o dist/ArchOpen src/main.cpp

test:
	./dist/ArchOpen.exe ./dist/testfile.nes