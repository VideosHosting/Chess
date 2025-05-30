#ifndef BOARD_H
#define BOARD_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include "piece.h"
#include "setting.h"

typedef struct Board {
    Piece_t* pieces;
    // Piece_t pieces[DIM_X * DIM_Y]; // an array to hold pieces on the board
    
    const char* fen;
} Board_t;


Board_t* InitBoard();

bool loadPieceTextures(SDL_Renderer* renderer, Board_t* board);

void drawBoard(SDL_Renderer* renderer);
void drawPieces(SDL_Renderer* renderer, Board_t* board);

void freeBoard(Board_t* board);

#endif // BOARD_H