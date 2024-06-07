#include "chip8.h"
#define WINDOW_SCALE 16
#define WINDOW_WIDTH (SCREEN_WIDTH * WINDOW_SCALE)
#define WINDOW_HEIGHT (SCREEN_HEIGHT * WINDOW_SCALE)
#include <SDL2/SDL.h>
#include <unistd.h>


SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* texture = NULL;

int initialize();
void closeDisplay();
int handleInput(unsigned char* keyboard);

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

        // emulate the cycle
        emulateCycle(&chip8);

        // handle input
        quit = handleInput(chip8.keyboard);

        // render the screen
        if (chip8.drawFlag) {
            SDL_UpdateTexture(texture, NULL, chip8.screen, SCREEN_WIDTH * sizeof(unsigned int));
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, NULL, NULL);
            SDL_RenderPresent(renderer);
        }

        usleep(1500);
    }
    
    closeDisplay();
    return 0;
}

int initialize() {
    // initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 0;
    }

    // create the window
    window = SDL_CreateWindow("CHIP-8 Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
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
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
    if (texture == NULL) {
        printf("Texture could not be created! SDL_Error: %s\n", SDL_GetError());
        return 0;
    }

    return 1;
}

void closeDisplay() {
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int handleInput(unsigned char* keyboard) {
    int quit = 0;
    
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch(e.type)
        {
            case SDL_QUIT:
                quit = 1;
                break;
            case SDL_KEYDOWN:
                switch (e.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        quit = 1;
                        break;
                    case SDLK_x:
                        keyboard[0] = 1;
                        break;
                    case SDLK_1:
                        keyboard[1] = 1;
                        break;
                    case SDLK_2:
                        keyboard[2] = 1;
                        break;
                    case SDLK_3:
                        keyboard[3] = 1;
                        break;
                    case SDLK_q:
                        keyboard[4] = 1;
                        break;
                    case SDLK_w:
                        keyboard[5] = 1;
                        break;
                    case SDLK_e:
                        keyboard[6] = 1;
                        break;
                    case SDLK_a:
                        keyboard[7] = 1;
                        break;
                    case SDLK_s:
                        keyboard[8] = 1;
                        break;
                    case SDLK_d:
                        keyboard[9] = 1;
                        break;    
                    case SDLK_z:
                        keyboard[0xA] = 1;
                        break;    
                    case SDLK_c:
                        keyboard[0xB] = 1;
                        break;    
                    case SDLK_4:
                        keyboard[0xC] = 1;
                        break;    
                    case SDLK_r:
                        keyboard[0xD] = 1;
                        break;    
                    case SDLK_f:
                        keyboard[0xE] = 1;
                        break;    
                    case SDLK_v:
                        keyboard[0xF] = 1;
                        break;          
                }
                break;
            case SDL_KEYUP:
                switch (e.key.keysym.sym) {
                    case SDLK_x:
                        keyboard[0] = 0;
                        break;
                    case SDLK_1:
                        keyboard[1] = 0;
                        break;
                    case SDLK_2:
                        keyboard[2] = 0;
                        break;
                    case SDLK_3:
                        keyboard[3] = 0;
                        break;
                    case SDLK_q:
                        keyboard[4] = 0;
                        break;
                    case SDLK_w:
                        keyboard[5] = 0;
                        break;
                    case SDLK_e:
                        keyboard[6] = 0;
                        break;
                    case SDLK_a:
                        keyboard[7] = 0;
                        break;
                    case SDLK_s:
                        keyboard[8] = 0;
                        break;
                    case SDLK_d:
                        keyboard[9] = 0;
                        break;    
                    case SDLK_z:
                        keyboard[0xA] = 0;
                        break;    
                    case SDLK_c:
                        keyboard[0xB] = 0;
                        break;    
                    case SDLK_4:
                        keyboard[0xC] = 0;
                        break;    
                    case SDLK_r:
                        keyboard[0xD] = 0;
                        break;    
                    case SDLK_f:
                        keyboard[0xE] = 0;
                        break;    
                    case SDLK_v:
                        keyboard[0xF] = 0;
                        break;          
                }
                break;
        }
    }
    return quit;
}