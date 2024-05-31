 /* CHIP-8 has 4KB memory (4096 bytes), from location 0x000 (0) to 0xFFF (4095):
  * + 0x000 (0) to 0x1FF (511) - CHIP-8 interpreter
  * + 0x200 (512) to 0xFFF (4095) - program memory (ETI 660 programs start at 0x600 (1536))*/
unsigned char memory[4096];
unsigned short opcode; // current opcode (opcodes are 2 bytes)
unsigned char V[16]; // 16 * 1 byte registers (VF is carry flag)
unsigned short I;
unsigned short PC;
bool screen[64 * 32];
 // timers
unsigned char delay_timer;
unsigned char sound_timer;
 // stack
