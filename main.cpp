#include <iostream>
#include <SDL.h>
#include <map>
#include <vector>
#include "map.h"


using namespace std;

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
        break;
    }
    return palautettava;
}

void piirraTausta(map<pair<int,int>, vector<SDL_Surface*>> piirrettavat, SDL_Renderer *ren)
{
    pair<int, int> pari;
    //käy läpi kaikki ruudulla näkyvät blockit
    for (int i = 0; i<10; i++){
        for (int j = 0; j<8; j++){
        pari = {i,j};
            for (int n = 0; n < piirrettavat[pari].size(); n++){
                //piirtää blockin kaikki kohteet
                piirra(piirrettavat[pari].at(n), ren, i*64, j*64, 64, 64);
            }
        }
    }
    return;
}

int main(int argc, char *argv[])
{

    //alustetaan SDL:n videon käsittely, ja jos se epäonnistuu, lopetetaan ohjelman suoritus
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_EVENTS) != 0){
        cout << "SDL_Init Error: " << SDL_GetError() << endl;
        return 1;
    }

    int muutos = 1;
    bool poistu = false;
    SDL_Event e;

    SDL_Window *win = SDL_CreateWindow("da Peli", 200, 200, 640, 512, SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE|SDL_WINDOW_INPUT_GRABBED|SDL_WINDOW_BORDERLESS);
    //jos ikkunan luominen epäonnistuu
    if (win == nullptr){
        cout << "SDL_CreateWindow Error: " << SDL_GetError() << endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

    //lataa kuvat
    SDL_Surface * taustaRuoho = SDL_LoadBMP("bmp/grass.bmp");
    SDL_Surface * pelaaja = SDL_LoadBMP("bmp/player.bmp");
    SDL_Surface * puu = SDL_LoadBMP("bmp/tree_S.bmp");
    SDL_Surface * kivi = SDL_LoadBMP("bmp/rock_S.bmp");
    SDL_Surface * taustaSora = SDL_LoadBMP("bmp/ground.bmp");

    //asetetaan kuvista väri rgb(0,255,255) läpinäkyväksi -> tausta on läpinäkyvä
    SDL_SetColorKey(pelaaja, SDL_TRUE, SDL_MapRGB(pelaaja->format, 0, 0xFF, 0xFF));
    SDL_SetColorKey(puu, SDL_TRUE, SDL_MapRGB(puu->format, 0, 0xFF, 0xFF));
    SDL_SetColorKey(kivi, SDL_TRUE, SDL_MapRGB(kivi->format, 0, 0xFF, 0xFF));

    //tyhjentää renderin -> piirto puhtaalta pöydältä
    SDL_RenderClear(ren);

    //luodaan vektori, jossa on kaikki käytettävät kuvat
    vector<SDL_Surface*> kuvat;
    kuvat.push_back(taustaRuoho);
    kuvat.push_back(taustaSora);
    kuvat.push_back(pelaaja);
    kuvat.push_back(puu);
    kuvat.push_back(kivi);

    pair<int, int> pari;
    map<pair<int,int>, vector<SDL_Surface*>> piirrettavat;

    //generoi mappi, tällä hetkellä randomisoi mapin
    map<pair<int,int>, int> kartta = generateMap();

    vector<SDL_Surface*> piirto;
    for (int i = 0; i<10; i++){
            for (int j = 0; j<8; j++){
                //piirtää taustan kartta-mapin tietojen perusteella
                piirto = maaritaPiirrettavat((kartta[{i,j}]), kuvat);

                //assosioidaan piirrettävät kuvat oikean blockin kanssa
                piirrettavat.insert(pair<pair<int,int>, vector<SDL_Surface*>>({i, j}, piirto));
            }
    }

    piirraTausta(piirrettavat, ren);

    while(poistu != true){

        //optimoitavissa threadeilla -> thread hoitamaan yksittäisen blockin piirtoa ja vapautuessaan ottaa seuraavan käsittelyyn
        //animaation kokeilu for-lause


        SDL_PumpEvents();
        while ( SDL_PollEvent(&e) != 0){
            //hiiren näppäintä painetaan ja painettava näppäin on vasen
            if( e.type == SDL_MOUSEBUTTONDOWN && SDL_GetMouseState(NULL, NULL) == SDL_BUTTON_LEFT){
                int x{0};
                int y{0};
                //hakee hiiren koordinaatit
                SDL_GetMouseState( &x, &y);
                piirraTausta(piirrettavat, ren);
                //pelaaja piirretään keskelle blockia, mitä on klikattu
                x = (x-(x%64));
                y = (y-(y%64));
                piirra(pelaaja, ren, x, y, 64, 64);
                muutos = 1;
            }
            if( e.type == SDL_KEYDOWN)
            {
                const Uint8 *state = SDL_GetKeyboardState(NULL);
                if( state[SDL_SCANCODE_ESCAPE]){
                    poistu = true;
                }
            }
        }
        //Update the screen
        SDL_RenderPresent(ren);

    }

    //odotus-funktio
    //SDL_Delay(1000);

    SDL_FreeSurface(pelaaja);
    SDL_FreeSurface(taustaRuoho);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
