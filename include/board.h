#ifndef BOARD_H
#define BOARD_H

#include <SDL2/SDL.h>
#include <stdbool.h>

#include "piece.h"
#include "setting.h"

typedef struct Board {
    Piece_t pieces[DIM_X * DIM_Y];
    const char* fen;       // FEN string representing the board state
    char turn;
} Board_t;

// Initialization functions
void InitBoard(Board_t* board);
void InitBoardFromFen(Board_t* board, const char* fen);

// Texture loading
bool loadPieceTextures(SDL_Renderer* renderer, Board_t* board);

// Drawing functions
void highlight_coord(int row, int col);
void unhighlight_coord();
void drawBoard(SDL_Renderer* renderer);
void drawHighlighted(SDL_Renderer* renderer);
void drawPieces(SDL_Renderer* renderer, Board_t* board);

// Debug / utility
void printBoard(Board_t* board);

// Board manipulation
Piece_t* getPiece(Board_t* board, int row, int col);
void movePiece(Board_t* board, Piece_t* piece, int nrow, int ncol);

// Cleanup
void freeBoard(Board_t* board);

#endif // BOARD_H
