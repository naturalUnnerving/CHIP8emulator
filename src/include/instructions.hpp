/* instruction definitions */

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

// 

void Chip8::OP_Dxyn()
{

}

void Chip8::OP_Ex9E()
{

}

void Chip8::OP_ExA1()
{

}

void Chip8::OP_Fx07()
{

}

void Chip8::OP_Fx0A()
{

}

void Chip8::OP_Fx15()
{

}

void Chip8::OP_Fx18()
{

}

void Chip8::OP_Fx1E()
{

}

void Chip8::OP_Fx29()
{

}

void Chip8::OP_Fx33()
{

}

void Chip8::OP_Fx55()
{

}

void Chip8::OP_Fx65()
{

}
