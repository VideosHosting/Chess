#ifndef MOVE_INTERNAL_H
#define MOVE_INTERNAL_H

#include "move.h"  // still include public declarations

// Internal-use-only macros

// NOT supported by MSVC
// #define AllocMem(size) ({ \
//     Move_t* ptr = (Move_t*)malloc((size) * sizeof(Move_t)); \
//     if (!ptr) { \
//         ERROR("Memory allocation failed for %zu moves", (size_t)(size)); \
//     } \
//     ptr; \
// })

// Returns NULL on allocation failure - callers must check return value
static inline Move_t* AllocMem(size_t size) {
    Move_t* ptr = (Move_t*)malloc(size * sizeof(Move_t));
    if (!ptr) {
        ERROR("Memory allocation failed for %zu moves", size);
        exit(EXIT_FAILURE);
    }
    return ptr;
}


#define CheckType(piece, Type, msg) \
    if ((piece)->type != Type) { \
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", msg); \
        return (MoveList_t){NULL, 0}; \
    }

#define Check(moves) \
    if (!(moves)) { \
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Memory allocation failed for moves."); \
        return (MoveList_t){NULL, 0}; \
    }

#define ReAllocAttempt(movelist) \
    Move_t* tmp = realloc((movelist).moves, (movelist).size * sizeof(Move_t)); \
    if (!tmp) { \
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Memory reallocation failed for moves"); \
        return movelist; \
    } \
    (movelist).moves = tmp;

#endif // MOVE_INTERNAL_H