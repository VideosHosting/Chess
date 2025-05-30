#include "board.h"
#include "setting.h"

void drawBoard(SDL_Renderer* renderer) {
    SDL_Rect rect;

    //115,149,82,255
    SDL_SetRenderDrawColor(renderer, 115, 149, 82, 255);
    for(int i = 0; i < DIM_Y; i++) {
        for(int j = (i%2==0); j < DIM_X; j+=2) {
            rect.x = j*COL;
            rect.y = i*ROW;

            rect.w = COL;
            rect.h = ROW;

            SDL_RenderFillRect(renderer, &rect);
        }
    }

    SDL_SetRenderDrawColor(renderer, 235, 236, 208, 255);
    for(int i = 0; i < DIM_Y; i++) {
        for(int j = i%2; j < DIM_X; j+=2) {
            rect.x = j*COL;
            rect.y = i*ROW;

            rect.w = COL;
            rect.h = ROW;

            SDL_RenderFillRect(renderer, &rect);
        }
    }
}