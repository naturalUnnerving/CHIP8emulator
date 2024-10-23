all:
	g++ -I src/include -L src/lib -o bin/CHIP8emulator src/main.cpp -lmingw32 -lSDL2main -lSDL2
