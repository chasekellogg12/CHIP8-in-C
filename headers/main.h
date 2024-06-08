#include <SDL2/SDL.h>
#include <unistd.h>

#define WINDOW_SCALE 16
#define WINDOW_WIDTH (SCREEN_WIDTH * WINDOW_SCALE)
#define WINDOW_HEIGHT (SCREEN_HEIGHT * WINDOW_SCALE)

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* texture = NULL;

int initialize();
void closeDisplay();
int handleInput(unsigned char* keyboard);