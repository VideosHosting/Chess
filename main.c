#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "setting.h"
#include "board.h"

int main() {

    if(SDL_Init(SDL_INIT_EVERYTHING)) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        // std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;

        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Chess", 100, 100, Width, Height, SDL_WINDOW_SHOWN);
    if(!window) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        // std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;

        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if(!renderer) {
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        // std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;

        SDL_DestroyWindow(window);
        SDL_Quit();

        return 1;
    }

    SDL_Event event;
    bool quit = false;

    // 1000 / fps = time taken for each frame
    // 1000 / 60 = 16.67ms
    int FPS = 60;


    while(!quit) {
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) {
                quit = true;
            }
        }
        // Logic goes here
        // ... 


        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Rendering goes here
        // ...
        drawBoard(renderer);

        SDL_RenderPresent(renderer);
        SDL_Delay(1000 / FPS); // Delay to maintain the frame rate
    }


    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    // SDL_Quit() is called automatically when the program exits
    // but it's good practice to call it explicitly
    // to clean up resources and free memory

    return 0;
}
