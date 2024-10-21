/* instruction definitions */
#ifndef INST_H
#define INST_H
#ifndef CHIP8_H
#define CHIP8_H
#include "Chip8.hpp"
#endif
// CLS: clear the display

void Chip8::OP_00E0()
{
	memset(video, 0, sizeof(video));
}

// RET: return from a subroutine

void Chip8::OP_00EE()
{
	--stackPointer;
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
    sp++;
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
	registers[Vx] = randByte(rng) & bt;
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
		uint8_t spriteBite = memory[index + row];
		for (unsigned int column = 0; column < 8; column++) {
			uint8_t spritePixel = spriteByte & (0x80u >> col);
			uint32_t* screenPixel = &video[screenX + col + (screenY + row) * VIDEO_WIDTH];
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

#endif
