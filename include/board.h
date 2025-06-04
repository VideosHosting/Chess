#ifndef BOARD_H
#define BOARD_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include "setting.h"
#include "piece.h"
#include "move.h"

typedef struct Board {
    Piece_t pieces[DIM_X * DIM_Y];
    char turn;

    MoveList_t history;
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
void UndoMove();
void movePiece(Board_t* board, Piece_t* piece, int nrow, int ncol);
void getFEN(Board_t* board, char buffer[]);

bool IsCheck(Board_t* board, PieceColor_t color);

// Cleanup
void freeBoard(Board_t* board);

#endif // BOARD_H
