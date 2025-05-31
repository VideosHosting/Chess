#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "setting.h"
#include "board.h"
#include "piece.h"

int main() {

    if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        // std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;

        return 1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("IMG_Init Error: %s\n", IMG_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Chess", 100, 100, Width, Height, SDL_WINDOW_SHOWN);
    if(!window) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateWindow Error: %s", SDL_GetError());
        // std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;

        SDL_Quit();
        IMG_Quit();

        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if(!renderer) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateRenderer Error: %s", SDL_GetError());
        // std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;

        SDL_DestroyWindow(window);
        SDL_Quit();
        IMG_Quit();

        return 1;
    }

    Board_t* Board = InitBoardFromFen("rnbqkb1r/ppN1pppp/5n2/8/3P4/8/PPP1PPPP/R1BQKBNR b KQkq - 0 3");
    if(!Board) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to initialize board");
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        IMG_Quit();
        return 1;
    }

    bool result = loadPieceTextures(renderer, Board);
    if(!result) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load all piece textures");
        freeBoard(Board);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        IMG_Quit();
        return 1;
    }

    // Piece_t whitePawn = CreatePiece(0, 0, PAWN, WHITE);
    // LoadPieceTexture(renderer, &whitePawn);

    // Piece_t blackPawn = CreatePiece(1, 0, PAWN, BLACK);
    // LoadPieceTexture(renderer, &blackPawn);

    // if(!whitePawn.texture || !blackPawn.texture) {
    //     FreePieceTexture(&whitePawn);
    //     FreePieceTexture(&blackPawn);
    //     printf("Failed to load piece texture\n");
    //     SDL_DestroyRenderer(renderer);
    //     SDL_DestroyWindow(window);
    //     SDL_Quit();
    //     IMG_Quit();
    //     return 1;
    // }

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
        drawPieces(renderer, Board);
        // DrawPiece(renderer, &whitePawn);
        // DrawPiece(renderer, &blackPawn);

        SDL_RenderPresent(renderer);
        SDL_Delay(1000 / FPS); // Delay to maintain the frame rate
    }

    freeBoard(Board);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    IMG_Quit();
    // SDL_Quit() is called automatically when the program exits
    // but it's good practice to call it explicitly
    // to clean up resources and free memory

    return 0;
}
