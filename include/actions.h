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
        //init: annetaan luokalle osoittimet tarvittaviin muuttujiin
        actions(const int* block_size_ptr, map<pair<int,int>, block>* kartta, map<pair<int,int>, vector<SDL_Surface*>>* piirrettavat,
                 const int* x_blocks, const int* y_blocks, vector<SDL_Surface*>* kuvat, TTF_Font* font, SDL_Color* tekstinVari);
        virtual ~actions();
        //funktio, joka piirt‰‰ valikon ruudulle
        void showOptions(map<SDL_Rect*, int>* valikkoMappi, vector<SDL_Surface*>* tekstit, SDL_Renderer *ren,
                          int* x, int* y, int* edellinen_x, int* edellinen_y);
        //tarkastaa, onko klikattu jotakin valikon kohdetta ja tekee sen mukaiset toiminnot
        void selectOption(map<SDL_Rect*, int>* valikkoMappi, player* pelaaja_ptr, map<pair<int,int>, block>* kartta, SDL_Renderer *ren,
                           int* x, int *y, int* edellinen_x, int* edellinen_y);
        //siirt‰‰ pelaajan klikatulle laatalle
        void movePlayer(SDL_Renderer *ren, int* pelaaja_x, int *pelaaja_y, int* edellinen_x, int* edellinen_y);

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
