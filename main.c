#include "chip8.h"
#define WINDOW_SCALE 10
#define WINDOW_WIDTH (SCREEN_WIDTH * WINDOW_SCALE)
#define WINDOW_HEIGHT (SCREEN_HEIGHT * WINDOW_SCALE)
#include <SDL2/SDL.h>


SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* texture = NULL;

int initialize();
void close();

int main() {
    // initialize the screen 
    if (!initialize()) {
        fprintf(stderr, "Failed to initialize!\n");
        exit(1);
    }   

    CHIP8 chip8;
    initializeEmulator(&chip8);
    loadROM(&chip8, "roms/BREAKOUT.ch8");

    int quit = 0;
    SDL_Event e;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT)
                quit = 1;
        }
        emulateCycle(&chip8);

        // render the screen
        SDL_UpdateTexture(texture, NULL, chip8.screen, SCREEN_WIDTH * sizeof(Uint32));
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);

        // handle input
    }
    
    close();
    return 0;
}

int initialize() {
    // initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 0;
    }

    // create the window
    window = SDL_CreateWindow("CHIP-8 Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return 0;
    }

    // create the renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return 0;
    }

    // create the texture
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB332, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
    if (texture == NULL) {
        printf("Texture could not be created! SDL_Error: %s\n", SDL_GetError());
        return 0;
    }

    return 1;
}

void close() {
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}