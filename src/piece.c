#include "piece.h"
#include "setting.h"
#include <SDL2/SDL_image.h>

static SDL_Rect PieceSize = { .w=COL_SIZE, .h=ROW_SIZE };

// load the texture from the file
static SDL_Texture* LoadTexture(SDL_Renderer* renderer, char* filename) {
    SDL_Surface* surface = IMG_Load(filename);
    if(!surface) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "IMG_Load Error: %s", IMG_GetError());
        return NULL;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface); // No longer needed
    if(!texture) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_CreateTextureFromSurface Error: %s", SDL_GetError());
        return NULL;
    }

    return texture;
}

static const char* PieceTypeToString(PieceType_t type) {
    switch(type) {
        case PAWN:   return "Pawn";
        case ROOK:   return "Rook";
        case KNIGHT: return "Knight";
        case BISHOP: return "Bishop";
        case QUEEN:  return "Queen";
        case KING:   return "King";
        case PIECE_NONE: return "None";
    }
}

// load the texture for a piece based on its type and color
void loadPieceTexture(SDL_Renderer* renderer, Piece_t* piece) {
    char filename[50] = {0};
    SDL_snprintf(filename, sizeof(filename), "%s%c%s.png", ImagesPath, piece->color, PieceTypeToString(piece->type));

    piece->texture = LoadTexture(renderer, filename);
}


Piece_t CreatePiece(int x, int y, PieceType_t type, PieceColor_t color) {
    Piece_t piece;
    piece.x = x;
    piece.y = y;
    piece.type = type;
    piece.color = color;
    piece.texture = NULL; // Initialize texture to NULL

    if(piece.x < 0 || piece.x >= DIM_X || piece.y < 0 || piece.y >= DIM_Y) {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Piece position (%d, %d) is out of bounds.", piece.x, piece.y);
    }

    return piece;
}

void InitPiece(Piece_t* piece, int x, int y, PieceType_t type, PieceColor_t color) {
    if(!piece) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Piece pointer is NULL. Cannot create piece.");
        return; // Return an empty piece
    }

    *piece = CreatePiece(x, y, type, color);

}

void drawPiece(SDL_Renderer* renderer, Piece_t* piece) {
    PieceSize.x = piece->x * COL_SIZE;
    PieceSize.y = piece->y * ROW_SIZE;
    
    if(!piece->texture) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Piece texture is NULL. Cannot draw piece at (%d, %d).", piece->x, piece->y);
        return;
    }

    SDL_RenderCopy(renderer, piece->texture, NULL, &PieceSize);
}

void freePieceTexture(Piece_t* piece) {
    if(piece->texture) {
        SDL_DestroyTexture(piece->texture);
        piece->texture = NULL; // Set to NULL after freeing
    }
}