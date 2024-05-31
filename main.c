#include <stdio.h>
#include <stdlib.h>

#define MEMORY_SIZE 4096
#define NUM_REGISTERS 16
#define STACK_SIZE 16
#define KEYBOARD_SIZE 16
#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32
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

int main() {
    // initialize the CHIP 8. 
    CHIP8 chip8;

    initialize(&chip8);
    
    // Then, run cycles of emultation.
    while (1) {
        // emulate a cycle

        // then display

        // then handle input
    }
    
    return 0;
}

void initialize(CHIP8 *chip8) {
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
    char fontSet[5 * 16] = {
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
    
    memcpy(chip8->memory, fontSet, sizeof(fontSet));

    // reset the timers
    chip8->soundTimer = 0;
    chip8->delayTimer = 0;
}