#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#define MEMORY_SIZE 4096
#define NUM_REGISTERS 16
#define STACK_SIZE 16
#define KEYBOARD_SIZE 16
#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32
#define NUM_FONTS 80

typedef struct {
    unsigned char memory[MEMORY_SIZE]; // 4k memory to work with
    unsigned char V[NUM_REGISTERS]; // 16 general purpose registers each of size 8 bits (the last of which should only be used by flags)
    unsigned short I; // 16 bit (short) register to store memory addresses
    unsigned char soundTimer; // soundTimer => also subtracts 1 at a rate of 60HZ. When non-zero, a sound plays. When 0, it deactivates
    unsigned char delayTimer; // delayTimer => just subtracts 1 at a rate of 60HZ. When 0, it deactivates
    unsigned short pc;// program counter => the currently executing address
    unsigned char sp; // stack pointer => points to the top of the stack
    unsigned short stack[STACK_SIZE]; // stores the address the interpreter should return to when done with a subroutine
    unsigned char keyboard[KEYBOARD_SIZE]; // 16 different keys
    unsigned char screen[SCREEN_WIDTH * SCREEN_HEIGHT]; // a pixel is one byte. Hence, we need 32 x 64 for our screen dimensions
} CHIP8;

void loadROM(CHIP8* chip8, const char* filename);
void initialize(CHIP8* chip8);
void emulateCycle(CHIP8* chip8);

int main() {
    // initialize the CHIP 8. 
    CHIP8 chip8;

    initialize(&chip8);
    loadROM(&chip8, "roms/BREAKOUT.ch8");

    // so now our memory has all these instructions from the ROM. We have to go thru them and for each, we decode them and 
    // basically, we start by executing what's at 0x200. This might tell us to jump to the opcode at 0x210 for instance. We just jump around like this.
    // a single opcode is 2 bytes. So, when we try to fetch the opcode at PC, we must fetch the byte at PC and combine it with the byte at PC+1 to get the opcode

    // PC is like "what's coming next"
    // the stack contains all the commands we haven't conducted yet, but we need to get around to
    // SP is the position in the stack
    // PC is the thing that's supposed to come next
        // place what PC is pointing to on top of the stack
        // then conduct the current operation
        // then conduct the top of the stack

    // the ROM file contains the opcodes (instructions) for the emulator to follow
    // the emulator must read these instructions from the ROM file, store them in space after the PC (program counter), then follow them
    
    // Then, run cycles of emultation.
    while (1) {
        // emulate a cycle
        emulateCycle(&chip8);

        // then display

        // then handle input

    }
    
    return 0;
}

void emulateCycle(CHIP8* chip8) {
    // a cycle is just ONE opcode. Fetch it, decode it, execute it.
    // to fetch, get the char at memory[pc]. Combine it with the char at memory[pc+1] (bc opcodes are 2 bytes, with the first as the address and second as command)
    // to combine it, use a bitwise operator. Each byte is 8 bits. Take the first byte, shift these bits to the left 4 spaces (to make room for the next byte).
        // then, use an OR between the two to create a short (2 byte) opcode
    unsigned short opcode = (chip8->memory[chip8->pc] << 8) | chip8->memory[chip8->pc+1];
    // 1111 0000000000 = 0xF000
    // F = 1111
    // 0 = 0000
    // etc (each hex digit is 4 bits)

    // x will always appear in the second position. This is an index.
    unsigned short x = (opcode & 0x0F00) >> 8; // 0x0F00 = 0000 1111 0000 0000 -> shift to the right 8 times to get just the index.

    // y will always appear in the third position. This is an index.
    unsigned short y = (opcode & 0x00F0) >> 4; // 0x00F0 = 0000 0000 1111 0000 -> shift to the right 4 times to get just the index.
    
    // y will always appear in the third position
    switch(opcode & 0xF000) { // at first, we only care about the first 4 bits of the opcode as the opcodes are grouped based on this first nibble
        case 0x0000:
            switch(opcode & 0x00FF) {
                case 0x00E0: // CLS - clear the display
                    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++)
                        chip8->screen[i] = 0;
                    chip8->pc += 2;
                    break;
                case 0x00EE: // RET - return from subroutine
                    chip8->sp--;
                    chip8->pc = chip8->stack[chip8->sp];
                    chip8->pc += 2; // pc is always the NEXT instruction
                    break;
                default: // 0nnn -> SYS addr - jump to a machine code routine at nnn. A JMP is like a call but it doesnt put anything onto the stack
                    // just set PC to be the called address + 2 (this is typically ignored by modern interpreters)
                    chip8->pc += 2;
                    break;
            }
            break;
        case 0x1000: // 1nnn -> JP addr - jump to location nnn
            chip8->pc = opcode & 0x0FFF;
            break;
        case 0x2000: // 2nnn -> CALL addr
            chip8->stack[chip8->sp] = chip8->pc;
            chip8->sp += 1;
            chip8->pc = opcode & 0x0FFF;
            break;
        case 0x3000: // 3xkk - SE Vx, byte (SE) - skip NEXT instruction if Vx == kk
            if (chip8->V[x] == (opcode & 0x00FF))
                chip8->pc += 2;
            chip8->pc += 2;
            break;
        case 0x4000: // 4xkk - skip next instruction if Vx != kk (SNE)
            if (chip8->V[x] != (opcode & 0x00FF))
                chip8->pc += 2;
            chip8->pc += 2;
            break;
        case 0x5000: // 5xy0 - skip next instruction if Vx == Vy (SE)
            if (chip8->V[x] == chip8->V[y])
                chip8->pc += 2;
            chip8->pc += 2;
            break;
        case 0x6000: // 6xkk - set Vx = kk (LD)
            chip8->V[x] = (opcode & 0x00FF);
            chip8->pc += 2;
            break;
        case 0x7000: // 7xkk - set Vx = Vx + kk
            chip8->V[x] += (opcode & 0x00FF);
            chip8->pc += 2;
            break;
        case 0x8000: 
            switch(opcode & 0x000F) {
                case 0x0000: // 8xy0 - stores the value of register Vy in register Vx
                    chip8->V[x] = chip8->V[y];
                    chip8->pc += 2;
                    break;
                case 0x0001: // 8xy1 - performs bitwise OR between values in Vx and Vy and stores result in Vx
                    chip8->V[x] |= chip8->V[y];
                    chip8->pc += 2;
                    break;
                case 0x0002: // 8xy2 - performs bitwise AND between values in Vx and Vy and stores result in Vx
                    chip8->V[x] &= chip8->V[y];
                    chip8->pc += 2;
                    break;
                case 0x0003: // 8xy3 - performs bitwise XOR between values in Vx and Vy and stores result in Vx
                    chip8->V[x] ^= chip8->V[y];
                    chip8->pc += 2;
                    break;
                case 0x0004: // 8xy4 - set Vx = Vx + Vy, set VF = 1 if carry, VF = 0 otherwise.
                    // if adding causes overflow, put 1 in carry
                    chip8->V[0xF] = 0;
                    if (chip8->V[x] + chip8->V[y] > 0xFF)
                        chip8->V[0xF] = 0;
                    chip8->V[x] += chip8->V[y];
                    chip8->pc += 2;
                    break;
                case 0x0005: // 8xy5 - set Vx = Vx - Vy, set VF = NOT borrow.
                    chip8->V[0xF] = 0;
                    if (chip8->V[x] > chip8->V[y])
                        chip8->V[0xF] = 1;
                    chip8->V[x] -= chip8->V[y];
                    chip8->pc += 2;
                    break;
                case 0x0006: // 8xy6 - Set Vx = Vx SHR 1 (shift right 1)
                    chip8->V[0xF] = (chip8->V[x] & 0x1);
                    chip8->V[x] >>= 1;
                    chip8->pc += 2;
                    break;
                case 0x0007: // 8xy7 - Set Vx = Vy - Vx, set VF = NOT borrow.
                    chip8->V[0xF] = 0;
                    if (chip8->V[y] > chip8->V[x])
                        chip8->V[0xF] = 1;
                    chip8->V[x] = chip8->V[y] - chip8->V[x];
                    chip8->pc += 2;
                    break; 
                case 0x000E: // 8xyE - Set Vx = Vx SHL 1.
                    chip8->V[0xF] = ((chip8->V[x] >> 7) & 0x1); // shifting to right 7 times puts most significant bit at the rightmost spot
                    chip8->V[x] <<= 1;
                    chip8->pc += 2;
                    break;                     
                default:
                    fprintf(stderr, "Unkown opcode\n");
                    exit(1);
            }
            break;
        case 0x9000: // 9xy0 - Skip next instruction if Vx != Vy.
            if (chip8->V[x] != chip8->V[y])
                chip8->pc += 2;
            chip8->pc += 2;
            break;
        case 0xA000: // Annn - Set I = nnn.
            chip8->I = opcode & 0x0FFF;
            chip8->pc += 2;
            break;
        case 0xB000: // Bnnn - Jump to location nnn + V0.
            chip8->pc = (opcode & 0x0FFF) + chip8->V[0];
            break;
        case 0xC000: // Cxkk - Set Vx = random byte AND kk.
            srand(time(NULL)); // Seed the random number generator
            unsigned char randomByte = (unsigned char) rand(); // rand() produces a random int, so casting it to a char gives a random char
            chip8->V[x] = randomByte & (opcode & 0x00FF);
            chip8->pc += 2;
            break;
        case 0xD000: // Dxyn - Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
            break;
        case 0xE000:
            switch(opcode & 0x00FF) {
                case 0x009E: // Ex9E
                    break;
                case 0x00A1: // ExA1
                    break;
                default:
                    fprintf(stderr, "Unkown opcode\n");
                    exit(1);
            }
            break;
        case 0xF000:
            switch(opcode & 0x00FF) {
                case 0x0007: // Fx07
                    break;
                case 0x000A: // Fx0A
                    break;
                case 0x0015: // Fx15
                    break;
                case 0x0018: // Fx18
                    break;
                case 0x001E: // Fx1E
                    break;
                case 0x0029: // Fx29
                    break;
                case 0x0033: // Fx33
                    break;
                case 0x0055: // Fx55
                    break;
                case 0x0065: // Fx65
                    break;
                default:
                    fprintf(stderr, "Unkown opcode\n");
                    exit(1);
            }
        default:
            fprintf(stderr, "Unkown opcode\n");
            exit(1);
    }

    if (chip8->soundTimer > 0)
        chip8->soundTimer -= 1;
    
    if (chip8->delayTimer > 0)
        chip8->delayTimer -= 1;
    
}

void loadROM(CHIP8* chip8, const char* filename) {
    // open the file, check for errors
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        fprintf(stderr, "Error opening the file\n");
        exit(1);
    }
    
    struct stat buf;
    stat(filename, &buf);
    size_t fileSize = buf.st_size;

    // read from the file onto memory starting at position 0x200
    size_t bytesRead = fread(chip8->memory+0x200, 1, MEMORY_SIZE - 0x200, file);

    fclose(file);
    if (bytesRead != fileSize) {
        fprintf(stderr, "Error reading file\n");
        exit(1);
    }
}

void initialize(CHIP8* chip8) {
    // set the addresses:
    chip8->pc = 0x200; // has to start at address 0x200 (because the space above this is for the interpreter)
    chip8->sp = 0; // starts at the top of the stack
    chip8->I = 0; // index register starts at top of stack

    // clear the display: (go thru all the pixels and set them to 0)
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        chip8->screen[i] = 0;
    }

    // clear the stack, registers, and keyboard
    for (int i = 0; i < STACK_SIZE; i++) {
        chip8->stack[i] = 0;
        chip8->V[i] = 0;
        chip8->keyboard[i] = 0;
    }

    // clear the memory
    for (int i = 0; i < MEMORY_SIZE; i++) {
        chip8->memory[i] = 0;
    }

    // load the fontset into the area of memory for the interpreter (between 0x000 and 0x1FF) 
    // we have 16 sprites to remember (0 - F). Each of these have 5 bytes. Hence, we have 5 * 16 = 80 total bytes worth of sprites
    char fontSet[NUM_FONTS] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0,  // 0
        0x20, 0x60, 0x20, 0x20, 0x70,  // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0,  // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0,  // 3
        0x90, 0x90, 0xF0, 0x10, 0x10,  // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0,  // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0,  // 6
        0xF0, 0x10, 0x20, 0x40, 0x40,  // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0,  // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0,  // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90,  // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0,  // B
        0xF0, 0x80, 0x80, 0x80, 0xF0,  // C
        0xE0, 0x90, 0x90, 0x90, 0xE0,  // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0,  // E
        0xF0, 0x80, 0xF0, 0x80, 0x80   // F
    };

    for (int i = 0; i < NUM_FONTS; i++) {
        chip8->memory[0x50 + i] = fontSet[i]; // start the fontset at address 0x50 (position 80)
    }

    // reset the timers
    chip8->soundTimer = 0;
    chip8->delayTimer = 0;
}