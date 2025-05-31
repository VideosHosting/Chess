#include "board.h"
#include "setting.h"

void drawBoard(SDL_Renderer* renderer) {
    SDL_Rect rect;

    //115,149,82,255
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
    SDL_Log("Loading FEN: %s", STARTING_POSITION);
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
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "FEN string did not fill the board completely. Expected %dx%d rows, got %dx%d.", DIM_X, DIM_Y, row + 1, col + 1);
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
            freePieceTexture(&board->pieces[i]);
        }
        free(board->pieces);
    }

    free(board);
}