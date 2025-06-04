#ifndef SETTING_H
#define SETTING_H

#define ImagesPath "Assets/Images/"

#define Width 800
#define Height 800

#define DIM_X 8
#define DIM_Y 8

#define ROW_SIZE (Height / DIM_Y)
#define COL_SIZE (Width / DIM_X)

#define STARTING_POSITION "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"


// Maximum possible moves on an empty board with optimal placement:
// Pawn   : 4   (move one forward, move two forward, capture left, capture right)
// Knight : 8
// Bishop : 13  (diagonally in all directions)
// Rook   : 14  (horizontally and vertically)
// Queen  : 27  (combined rook and bishop moves)
// King   : 8   (one square in any direction)
#define MAX_MOVES_PAWN   4
#define MAX_MOVES_KNIGHT 8
#define MAX_MOVES_BISHOP 13
#define MAX_MOVES_ROOK   14
#define MAX_MOVES_QUEEN  27
#define MAX_MOVES_KING   8


#define LOG(msg, ...) SDL_Log(msg, ##__VA_ARGS__)
#define ERROR(msg, ...) SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, msg, ##__VA_ARGS__)
#define WARN(msg, ...) SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, msg, ##__VA_ARGS__)

#endif // SETTING_H