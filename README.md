# 👾 CHIP-8 Emulator

## About

CHIP-8 is a simple, interpreted programming language which was first used on some do-it-yourself computer systems in the late 1970s and early 1980s. The COSMAC VIP, DREAM 6800, and ETI 660 computers are a few examples. These computers typically were designed to use a television as a display, had between 1 and 4K of RAM, and used a 16-key hexadecimal keypad for input. The interpreter took up only 512 bytes of memory, and programs, which were entered into the computer in hexadecimal, were even smaller. - [Cowgod's Chip-8 Technical Reference v1.0](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM)

### CHIP-8 Specifications

- 4,096 bytes of RAM (where the memory from 0x000 to 0x1FF is reserved for the interpreter)
- 16 general purpose registers (each of 8 bits)
- a special register (of 16 bits) to store memory addresses
- a sound timer (of 8 bits)
- a delay timer (of 8 bits)
- a stack of up to 16 memory addresses (each of 16 bits) to store the address the interpreter should return to when done with a subroutine
- a program counter (of 16 bits) to indicate the currently executing address
- a stack pointer (of 8 bits) to indicate the current place in the stack
- a 16-key hexadecimal keypad
- a 64x32-pixel monochrome display

See the chip8.h file for more information on how I represented these specifications.

## "Why?" (Motivation/Goal)

This CHIP-8 interpreter marks my first foray into the world of emulation. The project serves as both a learning experience and a stepping stone toward more ambitious emulator projects, such as developing an NES or GameBoy emulator in the future. My fascination with computer architecture began during my college years, where I had the opportunity to study and work with it extensively. The challenge of understanding and replicating the intricacies of a computing system has always intrigued me. This project is a manifestation of that interest, allowing me to apply my knowledge and skills while deepening my understanding of how emulators work. I thoroughly enjoyed the process of building this CHIP-8 interpreter and look forward to tackling more complex systems in the future!

## 🚀 Getting Started

`make` to compile

`./main <ROM PATH>` to run

For some ROMs to play around with, navigate to the 'roms' folder (see credit for the creation of these ROMs below).

### Dependencies

- SDL2 Library (latest version)
- Clang C Compiler

## Improvements

- Properly handle sound
- Use a function pointer instead of messy switch statements (see Austin Morlan's explaination below)

## Author

This CHIP-8 emulator was made by [Chase Kellogg](https://chasekellogg.com/).

## Acknowledgments

### Documentation & Articles

- [Chip-8 Technical Reference v1.0 (Cowgod)](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM)
- [CHIP-8 (Wikipedia)](https://en.wikipedia.org/wiki/CHIP-8)
- [Building a CHIP-8 Emulator [C++] (Austin Morlan)](https://austinmorlan.com/posts/chip8_emulator/)

### ROMs

- [Chip-8 Games Pack (Zophar's Domain)](https://www.zophar.net/pdroms/chip8/chip-8-games-pack.html)
- [test_opcode.ch8 (github/corax89)](https://github.com/corax89/chip8-test-rom/blob/master/test_opcode.ch8)