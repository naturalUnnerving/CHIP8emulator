#include <cstdint>
#include <cstring>
#include <fstream>
#include <chrono>
#include <random>
#include <iostream>
#include <string>
#include <SDL2/SDL.h>

const unsigned int START_ADDRESS = 0x200;
const unsigned int FONT_SIZE = 80;
const unsigned int FONTSET_START_ADDRESS = 0x50;
const unsigned int VIDEO_HEIGHT = 320;
const unsigned int VIDEO_WIDTH = 640;

// fontset table

uint8_t fontset[FONT_SIZE] = {
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

// Chip8 emulator class

class Chip8
{
	public:
		uint8_t registers[16] {};
		uint8_t memory[4096] {};
		uint16_t index {};
		uint16_t programCounter {};
		uint16_t stack[16] {};
		uint8_t stackPointer {};
		uint8_t delayTimer {};
		uint8_t soundTimer {};
		uint8_t keypad[16] {};
		uint32_t video[64*32] {};
		uint16_t opcode;
		std::mt19937 rng;
		std::uniform_int_distribution<> randByte;

		// constructor
		Chip8();

		// rom loader and cycle methods
		void load_ROM(char const* filename);
		void cycle();

		// instructions
		void OP_00E0();
		void OP_00EE();
		void OP_1nnn();
		void OP_2nnn();
		void OP_3xkk();
		void OP_4xkk();
		void OP_5xy0();
		void OP_6xkk();
		void OP_7xkk();
		void OP_8xy0();
		void OP_8xy1();
		void OP_8xy2();
		void OP_8xy3();
		void OP_8xy4();
		void OP_8xy5();
		void OP_8xy6();
		void OP_8xy7();
		void OP_8xyE();
		void OP_9xy0();
		void OP_Annn();
		void OP_Bnnn();
		void OP_Cxkk();
		void OP_Dxyn();
		void OP_Ex9E();
		void OP_ExA1();
		void OP_Fx07();
		void OP_Fx0A();
		void OP_Fx15();
		void OP_Fx18();
		void OP_Fx1E();
		void OP_Fx29();
		void OP_Fx33();
		void OP_Fx55();
		void OP_Fx65();
		void OP_NULL();

		// decode and execute instruction
		void decode();
};

// constructor

Chip8::Chip8()
{
	// initialize PC
	programCounter = START_ADDRESS;
	
	// load fonts into memory
	for (unsigned int i = 0; i < FONT_SIZE; ++i) {memory[FONTSET_START_ADDRESS + i] = fontset[i];}
	
	// initialize rng
	std::mt19937::result_type const seedval = std::rand();
	rng.seed(seedval);
	randByte = std::uniform_int_distribution<>(0, 255U);
}

/* instruction definitions */

// CLS: clear the display

void Chip8::OP_00E0()
{
	std::memset(video, 0, sizeof(video));
}

// RET: return from a subroutine

void Chip8::OP_00EE()
{
	stackPointer--;
	programCounter = stack[stackPointer];
}

// jump addr: set program counter to nnn

void Chip8::OP_1nnn()
{
    uint16_t address = opcode & 0x0FFFu;
    programCounter = address;
}

// call addr: call subroutine at nnn

void Chip8::OP_2nnn()
{
    uint16_t address = opcode & 0x0FFFu;
    stack[stackPointer] = programCounter;
    stackPointer++;
    programCounter = address;
}

// SE VX, byte: skio next instructionif Vx = kk

void Chip8::OP_3xkk()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t bt = opcode & 0x00FFu;
    if (registers[Vx] == bt) programCounter += 2;
}

// SNE Vx, byte: skip next instruction if Vx != kk

void Chip8::OP_4xkk()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t bt = opcode & 0x00FFu;
    if (registers[Vx] != bt) programCounter += 2;
}

// SE Vx, Vy: skip next instruction if Vx = Vy

void Chip8::OP_5xy0()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    if (registers[Vx] == registers[Vy]) programCounter += 2;
}

// LD Vx, byte: load kk into Vx

void Chip8::OP_6xkk()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t bt = opcode & 0x00FFu;
    registers[Vx] = bt;
}

// ADD Vx, byte: increment Vx by kk

void Chip8::OP_7xkk()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t bt = opcode & 0x00FFu;
    registers[Vx] += bt;
}

// LD Vx, Vy: set Vx equal to Vy

void Chip8::OP_8xy0()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    registers[Vx] = registers[Vy];
}

// OR Vx, Vy: set Vx equal to Vx OR Vy

void Chip8::OP_8xy1()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    registers[Vx] |= registers[Vy];
}

// AND Vx, Vy: set Vx equal to Vx AND Vy

void Chip8::OP_8xy2()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    registers[Vx] &= registers[Vy];
}

// XOR Vx, Vy: set Vx equal to Vx XOR Vy

void Chip8::OP_8xy3()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    registers[Vx] ^= registers[Vy];
}

// ADD Vx, Vy: Increment Vx by Vy and set VF to carry

void Chip8::OP_8xy4()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    uint16_t sum = registers[Vx] + registers[Vy];
    if (sum > 255U) {registers[0xF] = 1;} else {registers[0xF] = 0;}
    registers[Vx] = sum & 0xFFu;
}

// SUB Vx, Vy: Decrement Vx by Vy, set VF to 1 if Vx > Vy, 0 otherwise

void Chip8::OP_8xy5()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	if (registers[Vx] > registers[Vy]) {registers[0xF] = 1;} else {registers[0xF] = 0;}
	registers[Vx] -= registers[Vy];
}

// SHR Vx: Set VF to least significant digit of Vx, then divide Vx by 2 (right shift by 1)

void Chip8::OP_8xy6()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	registers[0xF] = registers[Vx] & 0x1u;
	registers[Vx] >>= 1;
}

// SUBN Vx, Vy: set Vx to Vy - Vx. if Vy > VX, set VF to 1, otherwise 0

void Chip8::OP_8xy7()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	if (registers[Vy] > registers[Vx]) {registers[0xF] = 1;} else {registers[0xF] = 0;}
	registers[Vx] = registers[Vy] - registers[Vx];
}

// SHL Vx {, Vy}: if the most significant bit of Vx is 1 set VF to 1 otherwise 0, then multiply Vx by 2 (left shift by 1)

void Chip8::OP_8xyE()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	registers[0xF] = registers[Vx] & 0x80u >> 7u;
	registers[Vx] <<= 1;
}

// SNE Vx, Vy: skip next instruction if Vx != Vy

void Chip8::OP_9xy0()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	if (registers[Vx] != registers[Vy]) programCounter += 2;
}

// LD I, addr: set I to nnn

void Chip8::OP_Annn()
{
	uint16_t address = opcode & 0x0FFFu;
	index = address;
}

// JP V0, addr: set program counter to V0 + nnn

void Chip8::OP_Bnnn()
{
	uint16_t address = opcode & 0x0FFFu;
	programCounter = address + registers[0];
}

// RND Vx, byte: set Vx to random byte AND kk

void Chip8::OP_Cxkk()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t bt = opcode & 0x00FFu;
	registers[Vx] = (uint8_t)randByte(rng) & bt;
}

// DRW Vx, Vy, nibble: display n bite sprite at memory location I at positon (Vx, Vy) on the screen, set VF = collision

void Chip8::OP_Dxyn()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	uint8_t bt = opcode & 0x000Fu;
	uint8_t screenX = registers[Vx] % VIDEO_WIDTH;
	uint8_t screenY = registers[Vy] % VIDEO_HEIGHT;
	registers[0xF] = 0;
	for (unsigned int row = 0; row < bt; row++) {
		uint8_t spriteByte = memory[index + row];
		for (unsigned int column = 0; column < 8; column++) {
			uint8_t spritePixel = spriteByte & (0x80u >> column);
			uint32_t* screenPixel = &video[screenX + column + (screenY + row) * VIDEO_WIDTH];
			if (spritePixel) {if(*screenPixel == 0xFFFFFFFF) registers[0xF] = 1; *screenPixel ^= 0xFFFFFFFF;}
		}
	}
}

// SKP VX: skip next instruction if pressed key has the value of Vx

void Chip8::OP_Ex9E()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t key = registers[Vx];
	if (keypad[key]) programCounter += 2;
}

// SKNP Vx: skip next instruction if pressed key does not have the value of Vx

void Chip8::OP_ExA1()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t key = registers[Vx];
	if (!keypad[key]) programCounter += 2;
}

// LD Vx, DT: set Vx to the delay timer value

void Chip8::OP_Fx07()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	registers[Vx] = delayTimer;
}

// LD Vx, K: wait for a key press then store the value of the key in Vx, decrease the program counter by 2 whenever
// a key press is not detected to loop the instruction

void Chip8::OP_Fx0A()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	if (keypad[0]) {
		registers[Vx] = 0;
	}
	else if (keypad[1]) {
		registers[Vx] = 1;
	}
	else if (keypad[2]) {
		registers[Vx] = 2;
	}
	else if (keypad[3]) {
		registers[Vx] = 3;
	}
	else if (keypad[4]) {
		registers[Vx] = 4;
	}
	else if (keypad[5]) {
		registers[Vx] = 5;
	}
	else if (keypad[6]) {
		registers[Vx] = 6;
	}
	else if (keypad[7]) {
		registers[Vx] = 7;
	}
	else if (keypad[8]) {
		registers[Vx] = 8;
	}
	else if (keypad[9]) {
		registers[Vx] = 9;
	}
	else if (keypad[10]) {
		registers[Vx] = 10;
	}
	else if (keypad[11]) {
		registers[Vx] = 11;
	}
	else if (keypad[12]) {
		registers[Vx] = 12;
	}
	else if (keypad[13]) {
		registers[Vx] = 13;
	}
	else if (keypad[14]) {
		registers[Vx] = 14;
	}
	else if (keypad[15]) {
		registers[Vx] = 15;
	}
	else {
		programCounter -= 2;
	}
}

// LD DT, Vx: set the delay timer to Vx

void Chip8::OP_Fx15()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	delayTimer = registers[Vx];
}

// LD ST, Vx: set the sound timer to Vx

void Chip8::OP_Fx18()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	soundTimer = registers[Vx];
}

// ADD I, Vx: increment I by Vx

void Chip8::OP_Fx1E()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	index += registers[Vx];
}

// LD F, Vx: set the index I to the charachter sprite located at Vx

void Chip8::OP_Fx29()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t location = registers[Vx];
	index = FONTSET_START_ADDRESS + (5 * location);
}

// LD B, Vx: use modulus to get the hudereds, tens and ones digit of Vx and store them at memory
// location I, I+1 and I+2 respectively

void Chip8::OP_Fx33()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t value = registers[Vx];
	memory[index + 2] = value % 10;
	value /= 10;
	memory[index + 1] = value % 10;
	value /= 10;
	memory[index] = value % 10;
}

// LD [I], Vx: store all registers in order up until Vx starting at memory location I

void Chip8::OP_Fx55()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	for (uint8_t i = 0; i <= Vx; i++) {
		memory[index + i] = registers[i];
	}
}

// LD Vx, [I]: read all registers in order up until Vx starting at memory location I

void Chip8::OP_Fx65()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	for (uint8_t i = 0; i <= Vx; i++) {
		registers[i] = memory[index + i];
	}
}

// null instruction

void Chip8::OP_NULL() {}

/* end instruction definitions */

// loads contents of a rom file

void Chip8::load_ROM(char const* filename)
{
	std::ifstream file(filename, std::ios::binary | std::ios::ate);
	if (file.is_open()) {
		std::streampos size = file.tellg();
		char* buffer = new char[size];
		
		file.seekg(0, std::ios::beg);
		file.read(buffer, size);
		file.close();
		
		for (long i = 0; i < size; ++i) {
			Chip8::memory[START_ADDRESS + i] = buffer[i];
		}
		delete[] buffer;
	}
}

// decode and execute instruction

void Chip8::decode()
{
	switch ((opcode & 0xF000u) >> 16u)
	{
		case 0x0:
		switch (opcode & 0x000Fu)
		{
			case 0x0: OP_00E0(); break;
			case 0xE: OP_00EE(); break;
			default: OP_NULL();
		} break;
		case 0x1: OP_1nnn(); break;
		case 0x2: OP_2nnn(); break;
		case 0x3: OP_3xkk(); break;
		case 0x4: OP_4xkk(); break;
		case 0x5: OP_5xy0(); break;
		case 0x6: OP_6xkk(); break;
		case 0x7: OP_7xkk(); break;
		case 0x8:
		switch (opcode & 0x000Fu)
		{
			case 0x0: OP_8xy0(); break;
			case 0x1: OP_8xy1(); break;
			case 0x2: OP_8xy2(); break;
			case 0x3: OP_8xy3(); break;
			case 0x4: OP_8xy4(); break;
			case 0x5: OP_8xy5(); break;
			case 0x6: OP_8xy6(); break;
			case 0x7: OP_8xy7(); break;
			case 0xE: OP_8xyE(); break;
			default: OP_NULL();
		} break;
		case 0x9: OP_9xy0(); break;
		case 0xA: OP_Annn(); break;
		case 0xB: OP_Bnnn(); break;
		case 0xC: OP_Cxkk(); break;
		case 0xD: OP_Dxyn(); break;
		case 0xE:
		switch (opcode & 0x000Fu)
		{
			case 0xE: OP_Ex9E(); break;
			case 0x1: OP_ExA1(); break;
			default: OP_NULL();
		} break;
		case 0xF:
		switch (opcode & 0x00FFu)
		{
			case 0x07: OP_Fx07(); break;
			case 0x0A: OP_Fx0A(); break;
			case 0x15: OP_Fx15(); break;
			case 0x18: OP_Fx18(); break;
			case 0x1E: OP_Fx1E(); break;
			case 0x29: OP_Fx29(); break;
			case 0x33: OP_Fx33(); break;
			case 0x55: OP_Fx55(); break;
			case 0x65: OP_Fx65(); break;
			default: OP_NULL();
		} break;
		default: OP_NULL();
	}
}

// cpu cycle

void Chip8::cycle()
{
	// fetch opcode
	opcode = (memory[programCounter] << 8u) | memory[programCounter + 1];

	// increment program counter
	programCounter += 2;

	// decode and execute
	decode();

	// decrement sound timer if not 0
	if (delayTimer > 0) delayTimer--;

	// decrement sound timer if not 0
	if (soundTimer > 0) soundTimer--;
}

// SDL2 display platform

class Display
{
	SDL_Window* window{};
	SDL_Renderer* renderer{};
	SDL_Texture* texture{};

	public:
		Display(char const* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight)
		{
			SDL_Init(SDL_INIT_VIDEO);
			window = SDL_CreateWindow(title, 0, 0, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
			texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, textureWidth, textureHeight);
		}

		~Display()
		{
			SDL_DestroyTexture(texture);
			SDL_DestroyRenderer(renderer);
			SDL_DestroyWindow(window);
			SDL_Quit();
		}

		void Update(void const* buffer, int pitch)
		{
			SDL_UpdateTexture(texture, nullptr, buffer, pitch);
			SDL_RenderClear(renderer);
			SDL_RenderCopy(renderer, texture, nullptr, nullptr);
			SDL_RenderPresent(renderer);
		}

		bool ProcessInput(uint8_t* keys, bool quit)
		{
			quit = false;

			SDL_Event event;

			while (SDL_PollEvent(&event))
			{
				switch (event.type)
				{
					case SDL_QUIT: quit = true; break;
					case SDL_KEYDOWN:
					{
						switch (event.key.keysym.sym)
						{
							case SDLK_ESCAPE: quit = true; break;
							case SDLK_x: keys[0] = 1; break;
							case SDLK_1: keys[1] = 1; break;
							case SDLK_2: keys[2] = 1; break;
							case SDLK_3: keys[3] = 1; break;
							case SDLK_q: keys[4] = 1; break;
							case SDLK_w: keys[5] = 1; break;
							case SDLK_e: keys[6] = 1; break;
							case SDLK_a: keys[7] = 1; break;
							case SDLK_s: keys[8] = 1; break;
							case SDLK_d: keys[9] = 1; break;
							case SDLK_z: keys[0xA] = 1; break;
							case SDLK_c: keys[0xB] = 1; break;
							case SDLK_4: keys[0xC] = 1; break;
							case SDLK_r: keys[0xD] = 1; break;
							case SDLK_f: keys[0xE] = 1; break;
							case SDLK_v: keys[0xF] = 1; break;
						}
					} break;

					case SDL_KEYUP:
					{
						switch (event.key.keysym.sym)
						{
							case SDLK_ESCAPE: quit = true; break;
							case SDLK_x: keys[0] = 0; break;
							case SDLK_1: keys[1] = 0; break;
							case SDLK_2: keys[2] = 0; break;
							case SDLK_3: keys[3] = 0; break;
							case SDLK_q: keys[4] = 0; break;
							case SDLK_w: keys[5] = 0; break;
							case SDLK_e: keys[6] = 0; break;
							case SDLK_a: keys[7] = 0; break;
							case SDLK_s: keys[8] = 0; break;
							case SDLK_d: keys[9] = 0; break;
							case SDLK_z: keys[0xA] = 0; break;
							case SDLK_c: keys[0xB] = 0; break;
							case SDLK_4: keys[0xC] = 0; break;
							case SDLK_r: keys[0xD] = 0; break;
							case SDLK_f: keys[0xE] = 0; break;
							case SDLK_v: keys[0xF] = 0; break;
						}
					} break;
				}
			}
			return quit;
		}
};

// main program loop

int main(int argc, char* argv[])
{
	// allow cycle delay, video scale and rom file args only
	if (argc != 4)
	{
		std::cerr << "Error: Invalid arguments\n\n" << "Usage: " << argv[0] << " <CPU cycle delay> <video scale> <ROM>\n";
		std::exit(EXIT_FAILURE);
	}

	// define cycle delay, video scale and rom filename
	int cycleDelay = std::stoi(argv[1]);
	int videoScale = std::stoi(argv[2]);
	char const *romFilename = argv[3];

	// initialize display
	char const* windowTitle = "CHIP8emulator - " + *romFilename;
	Display display(windowTitle, VIDEO_WIDTH * videoScale, VIDEO_HEIGHT * videoScale, VIDEO_WIDTH, VIDEO_HEIGHT);

	// initialize emulator and load rom file
	Chip8 emulator;
	emulator.load_ROM(romFilename);

	// set video pitch
	int videoPitch = sizeof(emulator.video[0]) * VIDEO_WIDTH;

	// calculate the time of one cpu cycle specific to the user's machine
	auto lastCycleTime = std::chrono::high_resolution_clock::now();

	// set program running status
	bool quit = false;

	// program loop
	while (!quit)
	{
		// set quit if display window is exited
		quit = display.ProcessInput(emulator.keypad, quit);

		// get a timestamp from user's machine and calculate the delay between timestamp and previous cycle time
		auto currentTime = std::chrono::high_resolution_clock::now();
		float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();

		// if real time delay is greater than the specified emulator cycle delay, process new emulator cycle
		if (dt > cycleDelay)
		{
			lastCycleTime = currentTime;
			emulator.cycle();
			display.Update(emulator.video, videoPitch);
		}
	}
	return EXIT_SUCCESS;
}
