#ifndef DRAW_H
#define DRAW_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <vector>
#include <map>
#include <iostream>

using namespace std;

//haetaan vakioarvojen pointterit
void alustaPiirto(const int*, const int*, const int*, const int*, const int*);
void piirra(SDL_Surface *kuvaT, SDL_Renderer *renT, int x, int y, int w, int h);
vector<SDL_Surface*> maaritaPiirrettavat(int tyyppi, vector<SDL_Surface*> kuvat);
void piirraTausta(map<pair<int,int>, vector<SDL_Surface*>> piirrettavat, SDL_Renderer *ren, pair<int, int> pari = {-1, -1});
SDL_Surface* lataaKuva(string path, bool lapinakyva);

#endif // DRAW_H
