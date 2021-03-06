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

void piirra(SDL_Texture *kuvaT, SDL_Renderer *renT, int x, int y, int w, int h){
    SDL_Rect alue;
    alue.x = x;
    alue.y = y;
    alue.w = w;
    alue.h = h;

    //render�ij�, tekstuuri, alkuper�isen kuvan alue, piirtoalue ikkunassa
    SDL_RenderCopy(renT, kuvaT ,NULL, &alue );
    return;
}

vector<SDL_Texture*> maaritaPiirrettavat(int tyyppi, vector<SDL_Texture*> kuvat)
{
    enum {ruoho, sora, pelaaja, puu, kivi, vesi};
    vector<SDL_Texture*> palautettava;
    switch(tyyppi)
    {
    //ruohikko
    case 0:
        palautettava = {kuvat.at(ruoho)};
        break;
    //sora
    case 1:
        palautettava = {kuvat.at(sora)};
        break;
    //puu
    case 2:
        palautettava = {kuvat.at(ruoho), kuvat.at(puu)};
        break;
    //kivi
    case 3:
        palautettava = {kuvat.at(sora), kuvat.at(kivi)};
        break;
    //vesi
    case 4:
        palautettava = {kuvat.at(vesi)};
        break;

    default:
        cout << "virheellinen arvo kuvien m��rittelyss�" << endl;
        break;
    }
    return palautettava;
}

void piirraTausta(map<pair<int,int>, vector<SDL_Texture*>> piirrettavat, SDL_Renderer *ren, pair<int, int> pari)
{
    //koko ruudun piirto
    if (pari.first == -1 && pari.second == -1){
        //tyhjent�� renderin -> piirto puhtaalta p�yd�lt�
        SDL_RenderClear(ren);

        //k�y l�pi kaikki ruudulla n�kyv�t blockit
        for (int i = 0; i<*x_blocks_ptr; i++){
            for (int j = 0; j<*y_blocks_ptr; j++){
                pari = {i,j};
                //piirr� kaikki koordinaatin objektit
                for (unsigned int n = 0; n < piirrettavat[pari].size(); n++){
                    //piirt�� blockin kaikki kohteet
                    piirra(piirrettavat[pari].at(n), ren, i*(*block_size_ptr), j*(*block_size_ptr), *block_size_ptr, *block_size_ptr);
                }
            }
        }
        piirra(piirrettavat[{-2,-2}].at(0), ren, 0, *y_blocks_ptr*(*block_size_ptr), (*window_width_ptr), *window_height_ptr % *block_size_ptr);
    }

    //UI:n p�ivitys
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

SDL_Texture* lataaKuva(SDL_Renderer* renT, string path, bool lapinakyva)
{
    SDL_Surface * kuva = SDL_LoadBMP(path.c_str());
    if( lapinakyva == true){
        //asetetaan kuvista v�ri rgb(0,255,255) l�pin�kyv�ksi -> tausta on l�pin�kyv�
        SDL_SetColorKey(kuva, SDL_TRUE, SDL_MapRGB(kuva->format, 0, 0xFF, 0xFF));
    }
    SDL_Texture * optimoitu = SDL_CreateTextureFromSurface(renT, kuva);
    SDL_FreeSurface(kuva);
    return optimoitu;
}
