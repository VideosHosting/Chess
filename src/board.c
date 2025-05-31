#include "board.h"
#include "setting.h"
#include <stdio.h>

static bool is_highlighted = false;
static SDL_Rect highlight_area = { .w = COL_SIZE, .h = ROW_SIZE }; 

void drawBoard(SDL_Renderer* renderer) {
    SDL_Rect rect;

    SDL_SetRenderDrawColor(renderer, 115, 149, 82, 255);
    for(int i = 0; i < DIM_Y; i++) {
        for(int j = (i%2==0); j < DIM_X; j+=2) {
            rect.x = j*COL_SIZE;
            rect.y = i*ROW_SIZE;

            rect.w = COL_SIZE;
            rect.h = ROW_SIZE;

            SDL_RenderFillRect(renderer, &rect);
        }
    }

    SDL_SetRenderDrawColor(renderer, 235, 236, 208, 255);
    for(int i = 0; i < DIM_Y; i++) {
        for(int j = i%2; j < DIM_X; j+=2) {
            rect.x = j*COL_SIZE;
            rect.y = i*ROW_SIZE;

            rect.w = COL_SIZE;
            rect.h = ROW_SIZE;

            SDL_RenderFillRect(renderer, &rect);
        }
    }
}

void highlight_coord(int row, int col) {
    // unhighlight
    if((highlight_area.x / COL_SIZE) == col && (highlight_area.y / ROW_SIZE) == row) {
        highlight_area.x = -1;
        highlight_area.y = -1;
        is_highlighted = false;
        return;
    }

    is_highlighted = true;

    highlight_area.x = col * COL_SIZE;
    highlight_area.y = row * ROW_SIZE;

    SDL_Log("Highlighting cell (%d, %d) at pixel (%d, %d)", row, col, highlight_area.x, highlight_area.y);
}

void drawHighlighted(SDL_Renderer* renderer) {
    if(!is_highlighted) return;

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 128); // semi-transparent red
    SDL_RenderFillRect(renderer, &highlight_area);
}

static PieceType_t GetPieceTypeByLetter(char c) {
    c = SDL_tolower(c);

    switch(c) {
        case 'p': return PAWN;
        case 'r': return ROOK;
        case 'n': return KNIGHT;
        case 'b': return BISHOP;
        case 'q': return QUEEN;
        case 'k': return KING;
    }
}

//rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
static void loadFen(const char* fen, Board_t* board) {
    SDL_Log("Loading FEN: %s", fen);
    int row = 0, col = 0;


    for(int i = 0; fen[i] != '\0' && fen[i] != ' ' && row < DIM_Y; ++i) {
        char c = fen[i];

        if(c == '/') {
            row++;
            col = 0;
        } else if(SDL_isdigit(c)) { // Empty squares
            int empty = c - '0';
            for(int j = 0; j < empty && col < DIM_X; ++j) {
                board->pieces[row * DIM_X + col].texture = NULL; // Empty square
                col++;
            }
        } else {
            
            InitPiece(board->pieces + (row * DIM_X + col),
                col, row, GetPieceTypeByLetter(c), (c >= 'a') ? BLACK : WHITE);

            col++;
        }
    }

    if(row < DIM_Y - 1 || col < DIM_X - 1) {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "FEN string did not fill the board properly. Expected %dx%d rows, got %dx%d.", DIM_X, DIM_Y, row + 1, col + 1);
    }
}

// Initialize the board with the starting position
Board_t* InitBoard() {
    return InitBoardFromFen(STARTING_POSITION);
}

Board_t* InitBoardFromFen(const char* fen) {
    Board_t* board = (Board_t*)malloc(sizeof(Board_t));
    if(!board) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to allocate memory for board.");
        return NULL;
    }

    board->pieces = (Piece_t*)malloc(DIM_X * DIM_Y * sizeof(Piece_t));
    if(!board->pieces) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to allocate memory for pieces.");
        free(board);
        return NULL;
    }

    // zero out the pieces array
    // so PieceType will be PIECE_NONE
    SDL_memset(board->pieces, 0, DIM_X * DIM_Y * sizeof(Piece_t));
    
    loadFen(fen, board);

    // Set the FEN string
    board->fen = STARTING_POSITION;

    return board;
}

void printBoard(Board_t* board) {
    if(!board || !board->pieces) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Board or pieces are NULL. Cannot print board.");
        return;
    }

    for(int i = 0; i < DIM_Y; i++) {
        for(int j = 0; j < DIM_X; j++) {
            Piece_t* piece = &board->pieces[i * DIM_X + j];
            if(piece->type == PIECE_NONE) {
                printf(". ");
            } else {

                printf("%c ", (piece->color == WHITE) ? SDL_toupper(piece->type) : piece->type);
            }
        }
        printf("\n");
    }
}

Piece_t* getPiece(Board_t* board, int row, int col) {
    Piece_t* piece = &board->pieces[row * DIM_X + col];


    if(piece->type == PIECE_NONE) {
        return NULL;
    }

    return piece;
}

void movePiece(Board_t* board, Piece_t* piece, int nrow, int ncol) {
    Piece_t* nPiece = &board->pieces[nrow * DIM_X + ncol];

    // Free texture if capturing
    if(nPiece->texture && nPiece->texture != piece->texture) {
        freePieceTexture(nPiece);
    }

    nPiece->texture = piece->texture;
    nPiece->color = piece->color;
    nPiece->type = piece->type;
    nPiece->x = ncol;
    nPiece->y = nrow;

    piece->texture = NULL;
    piece->type = PIECE_NONE;
    piece->color = 0;
    piece->x = -1;
    piece->y = -1;
}

bool loadPieceTextures(SDL_Renderer* renderer, Board_t* board) {
    if(!board || !board->pieces) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Board or pieces are NULL. Cannot load piece textures.");
        return false;
    }

    for(int i = 0; i < DIM_X * DIM_Y; i++) {
        Piece_t* piece = &board->pieces[i];
        if(piece->type != PIECE_NONE && piece->texture == NULL) {
            loadPieceTexture(renderer, piece);

            if(piece->texture == NULL) {

                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load texture for piece at (%d, %d).", piece->x, piece->y);
                return false;

            }
        }
    }

    return true;
}

void drawPieces(SDL_Renderer* renderer, Board_t* board) {
    if(!board || !board->pieces) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Board or pieces are NULL. Cannot draw pieces.");
        return;
    }

    for(int i = 0; i < DIM_X * DIM_Y; i++) {
        Piece_t* piece = &board->pieces[i];
        if(piece->texture) {
            drawPiece(renderer, piece);
        }
    }
}

void freeBoard(Board_t* board) {
    if(!board) return;

    if(board->pieces) {
        for(int i = 0; i < DIM_X * DIM_Y; i++) {
            if(board->pieces[i].texture) {
                freePieceTexture(&board->pieces[i]);
            }
        }
        free(board->pieces);
    }

    free(board);
}