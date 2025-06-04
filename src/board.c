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

void unhighlight_coord() {
    is_highlighted = false;
}

void highlight_coord(int row, int col) {

    is_highlighted = true;

    highlight_area.x = col * COL_SIZE;
    highlight_area.y = row * ROW_SIZE;

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
    int i = 0;

    for(; fen[i] != '\0' && fen[i] != ' ' && row < DIM_Y; ++i) {
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
        WARN("FEN string did not fill the board properly. Expected %dx%d rows, got %dx%d.", DIM_X, DIM_Y, row + 1, col + 1);
    }

    if(fen[i] == '\0') {
        return;
    }

    board->turn = fen[++i];

    //TODO: after fetching turn, next is castling, pawn moves, half moves & full moves
}

static void getKings(Board_t* board) {
    /* reset cached pointers before scanning */
    
    for(int row = 0; row < DIM_Y; row++) {
        for(int col = 0; col < DIM_X; col++) {
            Piece_t* target = getPiece(board, row, col);
            if(!target || target->type != KING) continue;

            if(target->color == WHITE)
                board->WhiteKing = target;
            else
                board->BlackKing = target;
        }
    }

    if(!board->WhiteKing || !board->BlackKing) {
        ERROR("King is missing!");
    }
}

// Initialize the board with the starting position
void InitBoard(Board_t* board) {
    return InitBoardFromFen(board, STARTING_POSITION);
}

void InitBoardFromFen(Board_t* board, const char* fen) {
    board->WhiteKing = NULL;
    board->BlackKing = NULL;
    board->history.moves = NULL;
    board->history.size = 0;
    
    SDL_memset(board->pieces, 0, DIM_X * DIM_Y * sizeof(Piece_t));

    loadFen(fen, board);

    getKings(board);
}

void printBoard(Board_t* board) {
    if(!board || !board->pieces) {
        ERROR("Board or pieces are NULL. Cannot print board.");
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

static inline bool WithinBounds(int row, int col) {
    return row >= 0 && row < DIM_Y &&
           col >= 0 && col < DIM_X;
}

// MoveList_t getAttackMoves(Board_t*, PieceColor_t);

Piece_t* getPiece(Board_t* board, int row, int col) {
    if(!WithinBounds(row, col)) { //avoid out of bounds shit (my stupidity might cause something)
        ERROR("Attempt to access out-of-bounds coords row (y): %d, col (x): %d", row, col);
        return NULL;
    }

    Piece_t* piece = &board->pieces[row * DIM_X + col];

    if(piece->type == PIECE_NONE) {
        return NULL;
    }

    return piece;
}

void movePiece(Board_t* board, Piece_t* piece, int nrow, int ncol) {
    Piece_t* nPiece = &board->pieces[nrow * DIM_X + ncol];
    if(nPiece == piece) {
        return;
    }
    // if(piece->color != board->turn) {
    //     return;
    // }
    if(piece->color == nPiece->color) {
        return;
    }
    Move_t move;
    InitMoveP(&move, piece, nrow, ncol, 0);
    
    if(!isValidMove( board, piece, &move )) {
        return;
    }

    // Free texture if capturing
    if(nPiece->type != PIECE_NONE) {
        freePieceTexture(nPiece);
    }

    // SDL_memcpy(nPiece, piece, sizeof(Piece_t));
    
    nPiece->type = PIECE_NONE;

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

    board->turn = (board->turn == 'w') ? 'b' : 'w';

    if(nPiece->type == KING) {
        if(nPiece->color == WHITE) {
            board->WhiteKing = nPiece;
        } else {
            board->BlackKing = nPiece;
        }
    }

    AddMoveM(&board->history, (MoveList_t){&move, 1});
}

bool loadPieceTextures(SDL_Renderer* renderer, Board_t* board) {
    if(!board || !board->pieces) {
        ERROR("Board or pieces are NULL. Cannot load piece textures.");
        return false;
    }

    for(int i = 0; i < DIM_X * DIM_Y; i++) {
        Piece_t* piece = &board->pieces[i];
        if(piece->type != PIECE_NONE && piece->texture == NULL) {
            loadPieceTexture(renderer, piece);

            if(piece->texture == NULL) {

                ERROR("Failed to load texture for piece at (%d, %d).", piece->x, piece->y);
                return false;

            }
        }
    }

    return true;
}

void drawPieces(SDL_Renderer* renderer, Board_t* board) {
    if(!board || !board->pieces) {
        ERROR("Board or pieces are NULL. Cannot draw pieces.");
        return;
    }

    for(int i = 0; i < DIM_X * DIM_Y; i++) {
        Piece_t* piece = &board->pieces[i];
        if(piece->texture) {
            drawPiece(renderer, piece);
        }
    }
}

void getFEN(Board_t* board, char buffer[]) {
    int size = 0;
    for (int row = 0; row < DIM_Y; row++) {
        int skip = 0;
        for (int col = 0; col < DIM_X; col++) {
            Piece_t* piece = &board->pieces[row * DIM_X + col];

            if (piece->type != PIECE_NONE) {
               if (skip > 0)
                    buffer[size++] = '0' + skip;

                buffer[size++] = (piece->color == WHITE) ? SDL_toupper(piece->type) : piece->type;
                skip = 0;
                
            } else
                skip++;

        }
        if (skip > 0)
            buffer[size++] = '0' + skip;

        if (row != DIM_Y - 1)
            buffer[size++] = '/';
    }
    buffer[size] = '\0';
}

static Move_t* pop(MoveList_t* history) {
    int lastOffset = history->size - 1;

    Move_t* latest = history->moves + lastOffset;

    history->size -= 1;
    // history->moves + lastOffset = NULL;

    return latest;
}

void UndoMove(Board_t *board) {
    if(board->history.size == 0) return;
    Move_t* latest = pop(&board->history);
    
    int from =  latest->to_row * DIM_X + latest->to_col;
    int to = latest->from_row * DIM_X + latest->from_col;
    // Piece_t* target = &board->pieces[latest->from_row * DIM_X + latest->from_col]; // old
    // Piece_t* target2 = &board->pieces[latest->to_row * DIM_X + latest->to_col]; // new

    SDL_memcpy(board->pieces + to, board->pieces + from, sizeof(Piece_t));

    Piece_t temp = board->pieces[to];
    board->pieces[to] = board->pieces[from];
    board->pieces[from] = temp;

}

void freeBoard(Board_t* board) {
    if(!board) return;

    if(board->pieces)
        for(int i = 0; i < DIM_X * DIM_Y; i++)
            if(board->pieces[i].texture)
                freePieceTexture(&board->pieces[i]);

    if(board->history.moves)
        free(board->history.moves);
}