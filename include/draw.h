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
//piirt‰‰ yksitt‰isen kohteen ruudulle
void piirra(SDL_Surface *kuvaT, SDL_Renderer *renT, int x, int y, int w, int h);
//m‰‰ritt‰‰ laatan kuvat laatan tyypin perusteella
vector<SDL_Surface*> maaritaPiirrettavat(int tyyppi, vector<SDL_Surface*> kuvat);
//piirt‰‰ kartan tai UI:n ruudulle
void piirraTausta(map<pair<int,int>, vector<SDL_Surface*>> piirrettavat, SDL_Renderer *ren, pair<int, int> pari = {-1, -1});
//funktio, jolla ladataan k‰ytett‰v‰t kuvat tiedostoista
SDL_Surface* lataaKuva(string path, bool lapinakyva);

#endif // DRAW_H
