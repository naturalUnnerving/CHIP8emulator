#include <cstdint>
#include <fstream>
#include <chrono>
#include <random>
#include "Chip8.hpp"
#include "fontset.hpp"
#include "instructions.hpp"

// constructor

Chip8::Chip8()
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

// cpu cycle
void Chip8::cycle()
{

}

// main program loop

int main()
{
	return 0;
}
