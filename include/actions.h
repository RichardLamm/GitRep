#ifndef ACTIONS_H
#define ACTIONS_H

#include <map>
#include <vector>
#include <draw.h>
#include "block.h"

using namespace std;

class actions
{
    public:
        actions(const int*, map<pair<int,int>, block>*, map<pair<int,int>, vector<SDL_Surface*>>*,
                const int*, const int*, vector<SDL_Surface*>*, TTF_Font*, SDL_Color*);
        virtual ~actions();

        void showOptions(map<SDL_Rect*, int>* valikkoMappi, vector<SDL_Surface*>* tekstit, SDL_Renderer *ren,
                         int* x, int* y, int* edellinen_x, int* edellinen_y);
        void selectOption(map<SDL_Rect*, int>* valikkoMappi, player* pelaaja_ptr, map<pair<int,int>, block>* kartta, SDL_Renderer *ren,
                          int* x, int *y, int* edellinen_x, int* edellinen_y);
        void movePlayer(SDL_Renderer *ren, int* x, int *y, int* edellinen_x, int* edellinen_y);

    private:
        const int* block_size_ptr_;
        map<pair<int,int>, block>* kartta_;
        int edellinenValikkoX_{-1};
        int edellinenValikkoY_{-1};
        map<pair<int,int>, vector<SDL_Surface*>>* piirrettavat_;
        const int* x_blocks_;
        const int* y_blocks_;
        vector<SDL_Surface*>* kuvat_;
        TTF_Font* font_;
        SDL_Color* tekstinVari_;
};

#endif // ACTIONS_H
