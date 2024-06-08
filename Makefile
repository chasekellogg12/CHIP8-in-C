# Compiler
CC = clang

# Executable
EXEC = main

# Source files
SRC = main.c chip8.c

# Compiler and linker flags
LDFLAGS = -F/Library/Frameworks -framework SDL2 -Wl,-rpath,/Library/Frameworks

# Default target
all: $(EXEC)

# Link object files to create the executable
$(EXEC):
	$(CC) -o $(EXEC) $(SRC) $(LDFLAGS)

# Clean target to remove generated files
clean:
	rm -f $(EXEC)

.PHONY: all clean
