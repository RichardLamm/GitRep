#include "draw.h"

using namespace std;

const int* block_size_ptr;
const int* window_height_ptr;
const int* window_width_ptr;
const int* x_blocks_ptr;
const int* y_blocks_ptr;

void alustaPiirto(const int* block_size, const int* window_height, const int* window_width, const int* x_blocks, const int* y_blocks){
    block_size_ptr = block_size;
    window_height_ptr = window_height;
    window_width_ptr = window_width;
    x_blocks_ptr = x_blocks;
    y_blocks_ptr = y_blocks;
}

void piirra(SDL_Surface *kuvaT, SDL_Renderer *renT, int x, int y, int w, int h){
    SDL_Rect alue;
    alue.x = x;
    alue.y = y;
    alue.w = w;
    alue.h = h;

    SDL_Texture *tex = SDL_CreateTextureFromSurface(renT, kuvaT);
    //renderöijä, tekstuuri, alkuperäisen kuvan alue, piirtoalue ikkunassa
    SDL_RenderCopy(renT, tex ,NULL, &alue );
    //SDL_RenderPresent(renT);
    return;
}

vector<SDL_Surface*> maaritaPiirrettavat(int tyyppi, vector<SDL_Surface*> kuvat)
{
    enum {ruoho, sora, pelaaja, puu, kivi};
    vector<SDL_Surface*> palautettava;
    switch(tyyppi)
    {
    case 0:
        palautettava = {kuvat.at(ruoho)};
        break;
    case 1:
        palautettava = {kuvat.at(sora)};
        break;
    case 2:
        palautettava = {kuvat.at(ruoho), kuvat.at(puu)};
        break;
    case 3:
        palautettava = {kuvat.at(sora), kuvat.at(kivi)};
        break;
    default:
        cout << "virheellinen arvo kuvien määrittelyssä" << endl;
        break;
    }
    return palautettava;
}

void piirraTausta(map<pair<int,int>, vector<SDL_Surface*>> piirrettavat, SDL_Renderer *ren, pair<int, int> pari)
{
    //koko ruudun piirto
    if (pari.first == -1 && pari.second == -1){
        //tyhjentää renderin -> piirto puhtaalta pöydältä
        SDL_RenderClear(ren);

        //käy läpi kaikki ruudulla näkyvät blockit
        for (int i = 0; i<*x_blocks_ptr; i++){
            for (int j = 0; j<*y_blocks_ptr; j++){
                pari = {i,j};
                //piirrä kaikki koordinaatin objektit
                for (unsigned int n = 0; n < piirrettavat[pari].size(); n++){
                    //piirtää blockin kaikki kohteet
                    piirra(piirrettavat[pari].at(n), ren, i*(*block_size_ptr), j*(*block_size_ptr), *block_size_ptr, *block_size_ptr);
                }
            }
        }
        piirra(piirrettavat[{-2,-2}].at(0), ren, 0, *y_blocks_ptr*(*block_size_ptr), (*window_width_ptr), *window_height_ptr % *block_size_ptr);
    }

    //UI:n päivitys
    else if (pari.first == -2 && pari.second == -2){
        piirra(piirrettavat[{-2,-2}].at(0), ren, 0, *y_blocks_ptr*(*block_size_ptr), (*window_width_ptr), *window_height_ptr % *block_size_ptr);
    }

    //osittainen piirto
    else{
        for (unsigned int n = 0; n < piirrettavat[pari].size(); n++){
            piirra(piirrettavat[pari].at(n), ren, pari.first*(*block_size_ptr), pari.second*(*block_size_ptr), *block_size_ptr, *block_size_ptr);
        }
    }

    return;
}

SDL_Surface* lataaKuva(string path, bool lapinakyva)
{
    SDL_Surface * kuva = SDL_LoadBMP(path.c_str());
    if( lapinakyva == true){
        //asetetaan kuvista väri rgb(0,255,255) läpinäkyväksi -> tausta on läpinäkyvä
        SDL_SetColorKey(kuva, SDL_TRUE, SDL_MapRGB(kuva->format, 0, 0xFF, 0xFF));
    }
    SDL_Surface * optimoitu = SDL_ConvertSurface(kuva, kuva->format, 0);
    return optimoitu;
}
