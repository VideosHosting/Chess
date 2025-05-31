#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "setting.h"
#include "board.h"

int main() {

    if(SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());

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

        SDL_Quit();
        IMG_Quit();

        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if(!renderer) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateRenderer Error: %s", SDL_GetError());

        SDL_DestroyWindow(window);
        SDL_Quit();
        IMG_Quit();

        return 1;
    }

    Board_t* board = InitBoardFromFen("rnbqkb1r/ppN1pppp/5n2/8/3P4/8/PPP1PPPP/R1BQKBNR b KQkq - 0 3");
    if(!board) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to initialize board");
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        IMG_Quit();
        return 1;
    }

    bool result = loadPieceTextures(renderer, board);
    if(!result) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load all piece textures");
        freeBoard(board);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        IMG_Quit();
        return 1;
    }

    SDL_Event event;
    bool quit = false;

    // 1000 / fps = time taken for each frame
    // 1000 / 60 = 16.67ms
    int FPS = 60;


    while(!quit) {
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                quit = true;
                break;

            case SDL_MOUSEBUTTONDOWN:
                int col = event.button.x / COL_SIZE,
                    row = event.button.y / ROW_SIZE;
                
                if(event.button.button == SDL_BUTTON_RIGHT) {

                    highlight_coord(row, col);


                } else if(event.button.button == SDL_BUTTON_LEFT) {
                    Piece_t* piece = getPiece(board, row, col);
                    

                    if(piece) {
                        movePiece(board, piece, rand()%8, rand()%8);
                        // piece->x = rand()%8;
                        // piece->y = rand()%8;
                    }

                    printBoard(board);
                }
            }
            // if(event.type == SDL_QUIT) {
            //     quit = true;
            // } else if(event.type == SDL_MOUSEBUTTONDOWN && ) {
            //     int col = event.button.x / COL_SIZE,
            //         row = event.button.y / ROW_SIZE;

            //     // printf("Pressed at %dx%d\n", x, y);

            //     highlight_coord(row, col);
            // }
        }


        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        drawBoard(renderer);
        drawHighlighted(renderer);
        drawPieces(renderer, board);

        SDL_RenderPresent(renderer);
        SDL_Delay(1000 / FPS); // Delay to maintain the frame rate
    }

    freeBoard(board);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    IMG_Quit();

    return 0;
}
