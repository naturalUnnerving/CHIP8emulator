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
		uint16_t pc {};
		uint16_t stack[16] {};
		uint8_t sp {};
		uint8_t delayTimer {};
		uint8_t soundTimer {};
		uint8_t keypad[16] {};
		uint32_t video[64*32] {};
		uint16_t opcode;
		std::default_random_engine randGen;
		std::uniform_int_distribution<uint8_t> randByte;
	
};

// loads contents of a ROM file

void Chip8::LoadROM(char const* filename) {
	
	std::ifstream file(filename, std::ios::binary | std::ios::ate);
	
	if (file.is_open()) {
		std::streampos size = file.tellg();
		char* buffer = new char[size];
		
		file.seekg(0, std::ios::beg);
		file.read(buffer, size);
		file.close();
		
		for (long i = 0; i < size; ++i) {
			memory[START_ADDRESS + i] = buffer[i];
		}
		
		delete[] buffer;
	}
}

// initialize

Chip8::Chip8() {
	// initialize PC
	pc = START_ADDRESS;
	
	// load fonts into memory
	for (unsigned int i = 0; i < FONT_SIZE; ++i) {
		memory[FONT_START_ADDRESS + i] = fontset[i];
	}
	
	randGen = std::chrono::system_clock::now().time_since_epoch().count();
	randByte = std::uniform_int_distribution<uint8_t>(0, 255U);
	
}

// main program loop

int main() {
	
	
	return 0;
}