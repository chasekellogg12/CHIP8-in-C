#include "chip8.h"

void emulateCycle(CHIP8* chip8) {
    /* 
        - a cycle is just ONE opcode. Fetch it, decode it, execute it.
        - to fetch:
            - get the char at memory[pc]
        - to decode:
            - combine it with the char at memory[pc+1] (bc opcodes are 2 bytes, with the first as the address and second as command)
                - to combine:
                    - 5ake the first byte, shift these bits to the left 4 spaces (to make room for the next byte)
                    - then, use an OR between the two to create a short (2 byte) opcode
        - to execute:
            - see implementations in the switch cases below
    */
    unsigned short opcode = (chip8->memory[chip8->pc] << 8) | chip8->memory[chip8->pc+1]; // fetch and decode

    chip8->drawFlag = 0;

    // x will always appear in the second position. This is an index.
    unsigned short x = (opcode & 0x0F00) >> 8; // 0x0F00 = 0000 1111 0000 0000 -> shift to the right 8 times to get just the index.

    // y will always appear in the third position. This is an index.
    unsigned short y = (opcode & 0x00F0) >> 4; // 0x00F0 = 0000 0000 1111 0000 -> shift to the right 4 times to get just the index.

    unsigned char randomByte; // we'll use this variable in opcdoe cxkk
    
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
                    printf("1");
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
            //srand(time(NULL)); // Seed the random number generator
            randomByte = (unsigned char) rand(); // rand() produces a random int, so casting it to a char gives a random char
            chip8->V[x] = randomByte & (opcode & 0x00FF);
            chip8->pc += 2;
            break;
        case 0xD000: // Dxyn - Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
            // given a sprite (the bytes for which are located at memory[I]), go thru the 8 bits of each of its bytes
            // if this bit is a 1, do screen pixel ^= 0xFFFFFFFFFF (which is just xoring it by 1)
            chip8->drawFlag = 1;
            chip8->V[0xF] = 0;
            unsigned char xPos = chip8->V[x] % SCREEN_WIDTH;
            unsigned char yPos = chip8->V[y] % SCREEN_HEIGHT;

            // the first row of the sprite is located at memory[i], second row located at memory[i+1], etc. [each row is a byte]
            for (int row = 0; row < (opcode & 0x000F); row++) { 
                unsigned char spriteByte = chip8->memory[chip8->I + row]; // gets the start of the rowth byte in the sprite
                for (int col = 0; col < 8; col++) { // go thru all the 8 bits in this byte of the sprite (shift spriteByte i bits to right then do & 1 to get the ith bit)
                    unsigned char spritePixel = (spriteByte >> col) & 0x01;
                    unsigned int* screenPixel = &chip8->screen[(yPos + row) * SCREEN_WIDTH + (xPos + col)]; // get what's currently at the position we need to be at
                    if (spritePixel) { // if the spritePixel is on
                        if (*screenPixel)
                            chip8->V[0xF] = 1;
                        *screenPixel ^= 0xFFFFFFFF;
                    }
                }
            }
            chip8->pc += 2;
            break;
        case 0xE000:
            switch(opcode & 0x00FF) {
                case 0x009E: // Ex9E - Skip next instruction if key with the value of Vx is pressed.
                    if (chip8->keyboard[chip8->V[x]])
                        chip8->pc += 2;
                    chip8->pc += 2;
                    break;
                case 0x00A1: // ExA1 - Skip next instruction if key with the value of Vx is not pressed.
                    if (!chip8->keyboard[chip8->V[x]])
                        chip8->pc += 2;
                    chip8->pc += 2;
                    break;
                default:
                    fprintf(stderr, "Unkown opcode\n");
                    printf("2");
                    exit(1);
            }
            break;
        case 0xF000:
            switch(opcode & 0x00FF) {
                case 0x0007: // Fx07 - Set Vx = delay timer value.
                    chip8->V[x] = chip8->delayTimer;
                    chip8->pc += 2;
                    break;
                case 0x000A: // Fx0A - Wait for a key press, store the value of the key in Vx.
                    // go thru all the values in keyboard until we find a pressed value
                    for (int i = 0; i < KEYBOARD_SIZE; i++) {
                        if (chip8->keyboard[i]) {
                            chip8->V[x] = i;
                            break;
                        }
                    }
                    chip8->pc += 2;
                    break;
                case 0x0015: // Fx15 - Set delay timer = Vx.
                    chip8->delayTimer = chip8->V[x];
                    chip8->pc += 2;
                    break;
                case 0x0018: // Fx18 - Set sound timer = Vx.
                    chip8->soundTimer = chip8->V[x];
                    chip8->pc += 2;
                    break;
                case 0x001E: // Fx1E - The values of I and Vx are added, and the results are stored in I.
                    chip8->I += chip8->V[x];
                    chip8->pc += 2;
                    break;
                case 0x0029: // Fx29 - Set I = location of sprite for digit Vx.
                    // V[x] is a char (0 - F). Set I = location of the V[x]th sprite. Each sprite is 5 bytes long. 
                    // the bytes are stored starting at memory[0x50]. So to find the start of the V[x]th sprite, do 0x50 + V[x] * 5
                    chip8->I = 0x50 + chip8->V[x] * 5;
                    chip8->pc += 2;
                    break;
                case 0x0033: // Fx33 - Store BCD representation of Vx in memory locations I, I+1, and I+2.
                    chip8->memory[chip8->I] = (chip8->V[x] % 1000) / 100;
                    chip8->memory[chip8->I+1] = (chip8->V[x] % 100) / 10;
                    chip8->memory[chip8->I+2] = (chip8->V[x] % 10);
                    chip8->pc += 2;
                    break;
                case 0x0055: // Fx55 - Store registers V0 through Vx in memory starting at location I.
                    for (int i = 0; i <= x; i++)
                        chip8->memory[chip8->I+i] = chip8->V[i];
                    chip8->pc += 2;
                    break;
                case 0x0065: // Fx65 - Read registers V0 through Vx from memory starting at location I.
                    for (int i = 0; i <= x; i++)
                        chip8->V[i] = chip8->memory[chip8->I+i];
                    chip8->pc += 2;
                    break;
                default:
                    fprintf(stderr, "Unkown opcode\n");
                    exit(1);
            }
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

void initializeEmulator(CHIP8* chip8) {
    // set the addresses:
    chip8->pc = 0x200; // has to start at address 0x200 (because the space above this is for the interpreter)
    chip8->sp = 0; // starts at the top of the stack
    chip8->I = 0; // index register starts at top of stack
    chip8->drawFlag = 0;

    // clear the display: (go thru all the pixels and set them to 0)
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++)
        chip8->screen[i] = 0;

    // clear the stack, registers, and keyboard
    for (int i = 0; i < STACK_SIZE; i++) {
        chip8->stack[i] = 0;
        chip8->V[i] = 0;
        chip8->keyboard[i] = 0;
    }

    // clear the memory
    for (int i = 0; i < MEMORY_SIZE; i++)
        chip8->memory[i] = 0;

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

    for (int i = 0; i < NUM_FONTS; i++)
        chip8->memory[0x50 + i] = fontSet[i]; // start the fontset at address 0x50 (position 80)

    // reset the timers
    chip8->soundTimer = 0;
    chip8->delayTimer = 0;
}