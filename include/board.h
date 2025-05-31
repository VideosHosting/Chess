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
Board_t* InitBoardFromFen(const char* fen);

bool loadPieceTextures(SDL_Renderer* renderer, Board_t* board);

void highlight_coord(int row, int col);

void drawBoard(SDL_Renderer* renderer);
void drawHighlighted(SDL_Renderer* renderer);
void drawPieces(SDL_Renderer* renderer, Board_t* board);

void printBoard(Board_t* board);

Piece_t* getPiece(Board_t* board, int row, int col);

void movePiece(Board_t* board, Piece_t* piece, int nrow, int ncol);

void freeBoard(Board_t* board);

#endif // BOARD_H