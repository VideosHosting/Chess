#ifndef PIECE_H
#define PIECE_H
#include <SDL2/SDL.h>

typedef enum PieceType {
    PIECE_NONE = 0,
    PAWN = 'p',
    ROOK = 'r',
    KNIGHT = 'n',
    BISHOP = 'b',
    QUEEN = 'q',
    KING = 'k'
} PieceType_t;

typedef enum PieceColor {
    WHITE = 'w',
    BLACK = 'b'
} PieceColor_t;

typedef struct Piece {
    unsigned int x, y;
    SDL_Texture* texture; // Pointer to the texture for rendering the piece
    PieceType_t type; // Type of the piece (e.g., pawn, knight, etc.)
    PieceColor_t color; // Color of the piece (e.g., white or black)
} Piece_t;

// Initialization functions
Piece_t CreatePiece(int x, int y, PieceType_t type, PieceColor_t color);
void InitPiece(Piece_t* piece, int x, int y, PieceType_t type, PieceColor_t color);

void loadPieceTexture(SDL_Renderer* renderer, Piece_t* piece);
void drawPiece(SDL_Renderer* renderer, Piece_t* piece);
void freePieceTexture(Piece_t* piece);

#endif // PIECE_H