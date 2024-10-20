/* instructions */

// CLS

void Chip8::OP_00E0() {
	memset(video, 0, sizeof(video));
}

// RET

void Chip8::OP_00EE() {
	--sp;
	pc = stack[sp];
}

