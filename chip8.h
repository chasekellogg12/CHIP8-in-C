#ifndef CHIP8_H
#define CHIP8_H

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
    unsigned int screen[SCREEN_WIDTH * SCREEN_HEIGHT]; // use an unsigned int for screen instead of char to make working with SDL easier
} CHIP8;

void loadROM(CHIP8* chip8, const char* filename);
void initializeEmulator(CHIP8* chip8);
void emulateCycle(CHIP8* chip8);

#endif