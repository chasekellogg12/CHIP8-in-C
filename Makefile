CC = clang
CFLAGS = -Wall -Wextra -std=c11 -I/Library/Frameworks/SDL2.framework/Headers
LDFLAGS = -F/Library/Frameworks -framework SDL2 -Wl,-rpath,/Library/Frameworks

SRC = main.c chip8.c
OBJ = $(SRC:.c=.o)
EXEC = chip8

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(EXEC)

.PHONY: clean
