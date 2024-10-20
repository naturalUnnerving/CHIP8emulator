#include <cstdint>
#include <fstream>
#include <chrono>
#include <random>
#include "fontset.hpp"
#include "instructions.hpp"

const unsigned int START_ADDRESS = 0x200;

class Chip8 {
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
		std::uniform_int_distribution<uint8_t> randByte;

		// constructor
		Chip8()
		{
			// initialize PC
			programCounter = START_ADDRESS;

			// load fonts into memory
			for (unsigned int i = 0; i < FONT_SIZE; ++i) {memory[FONT_START_ADDRESS + i] = fontset[i];}
			
			// initialize rng
			std::mt19937::result_type const seedval = std::rand();
			rng.seed(seedval);
			randByte = std::uniform_int_distribution<uint8_t>(0, 255U);
		}

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
};

// loads contents of a ROM file

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

// cpu cycle
void Chip8::cycle()
{

}

// main program loop

int main()
{
	return 0;
}
