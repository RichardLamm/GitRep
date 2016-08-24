#include <iostream>
#include <SDL.h>
#include <map>
#include <vector>
#include "map.h"
#include "block.h"
#include <string>
#include <SDL_ttf.h>

/*TODO:
    -valikko
    -yleinen päällepiirto -> ei jää "haamukuvia"
    -piirtojärjestys mappiin (depth) ja uudenlainen piirtofunktio?
    -oikeasti jonkinlaista interactaamista palikoiden kanssa
    -kartan laajempi generoiminen
    -pelaajan modelin muuttaminen
    -lisää kohteita maastoon
*/

//constit
const int block_size = 64;
const int window_height = 1080;
const int window_width = 1920;
const int x_blocks = (window_width-(window_width%64))/64;
const int y_blocks = (window_height-(window_height%64))/64;


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

void piirraTausta(map<pair<int,int>, vector<SDL_Surface*>> piirrettavat, SDL_Renderer *ren, pair<int, int> pari = {-1, -1})
{

    if (pari.first == -1 && pari.second == -1){
    //tyhjentää renderin -> piirto puhtaalta pöydältä
    SDL_RenderClear(ren);

    //käy läpi kaikki ruudulla näkyvät blockit
    for (int i = 0; i<x_blocks; i++){
        for (int j = 0; j<y_blocks; j++){
        pari = {i,j};
            for (int n = 0; n < piirrettavat[pari].size(); n++){
                //piirtää blockin kaikki kohteet
                piirra(piirrettavat[pari].at(n), ren, i*64, j*64, 64, 64);
            }
        }
    }
    piirra(piirrettavat[{-1,-1}].at(0), ren, 0, y_blocks*block_size, window_width, window_height % 64);
    }
    else{
        for (int n = 0; n < piirrettavat[pari].size(); n++){
            piirra(piirrettavat[pari].at(n), ren, pari.first*64, pari.second*64, 64, 64);
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
    SDL_Surface * optimoitu = SDL_ConvertSurface(kuva, kuva->format, NULL);
    return optimoitu;
}

int main(int argc, char *argv[])
{

    //alustetaan SDL:n videon käsittely, ja jos se epäonnistuu, lopetetaan ohjelman suoritus
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_EVENTS) != 0){
        cout << "SDL_Init Error: " << SDL_GetError() << endl;
        return 1;
    }
    TTF_Init();
    SDL_Color tekstinVari;
    tekstinVari.b = 255;
    TTF_Font* font = TTF_OpenFont("ARIAL.TTF", 15);
    //TTF_SetFontOutline(font, 1);
    int FontX;
    int FontY;
    SDL_Surface* teksti;

    int pelaaja_x;
    int pelaaja_y;
    int muutos = 1;
    int edellinen_x = -1;
    int edellinen_y = -1;
    bool poistu = false;
    bool valikko = false;
    SDL_Event e;

    SDL_Window *win = SDL_CreateWindow("da Peli", 0, 0, window_width, window_height, SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE|SDL_WINDOW_INPUT_GRABBED|SDL_WINDOW_BORDERLESS);
    //jos ikkunan luominen epäonnistuu
    if (win == nullptr){
        cout << "SDL_CreateWindow Error: " << SDL_GetError() << endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

    //lataa kuvat
    SDL_Surface * taustaRuoho = lataaKuva("bmp/grass.bmp", false);
    SDL_Surface * pelaaja = lataaKuva("bmp/player.bmp", true);
    SDL_Surface * puu = lataaKuva("bmp/tree_S.bmp", true);
    SDL_Surface * kivi = lataaKuva("bmp/rock_S.bmp", true);
    SDL_Surface * taustaSora = lataaKuva("bmp/ground.bmp", false);
    SDL_Surface * kirves = lataaKuva("bmp/axe.bmp", true);
    SDL_Surface * UI = lataaKuva("bmp/UI.bmp", false);


    //luodaan vektori, jossa on kaikki käytettävät kuvat
    vector<SDL_Surface*> kuvat;
    kuvat.push_back(taustaRuoho);
    kuvat.push_back(taustaSora);
    kuvat.push_back(pelaaja);
    kuvat.push_back(puu);
    kuvat.push_back(kivi);

    pair<int, int> pari;
    map<pair<int,int>, vector<SDL_Surface*>> piirrettavat;

    piirrettavat.insert(pair<pair<int,int>, vector<SDL_Surface*>>({-1,-1}, {UI}));

    //generoi mappi, tällä hetkellä randomisoi mapin
    map<pair<int,int>, block> kartta = generateMap(x_blocks, y_blocks);

    vector<SDL_Surface*> piirto;
    for (int i = 0; i<x_blocks; i++){
            for (int j = 0; j<y_blocks; j++){
                //piirtää taustan kartta-mapin tietojen perusteella
                pair<int, int> kohta {i,j};
                piirto = maaritaPiirrettavat((kartta.at(kohta).getType()), kuvat);
                kartta.at(kohta).SetImages(piirto);
                //assosioidaan piirrettävät kuvat oikean blockin kanssa
                piirrettavat.insert(pair<pair<int,int>, vector<SDL_Surface*>>({i, j}, piirto));
            }
    }

    piirraTausta(piirrettavat, ren);

    while(poistu != true){

        //optimoitavissa threadeilla -> thread hoitamaan yksittäisen blockin piirtoa ja vapautuessaan ottaa seuraavan käsittelyyn




        SDL_PumpEvents();
        while ( SDL_PollEvent(&e) != 0){
            //hiiren näppäintä painetaan ja painettava näppäin on vasen
            if( e.type == SDL_MOUSEBUTTONDOWN){
                if(SDL_GetMouseState(NULL, NULL) != SDL_BUTTON_LEFT){
                    int x;
                    int y;
                    SDL_GetMouseState( &x, &y);
                    SDL_Rect loota;
                    loota.x = x-(x%64)+10;
                    loota.h = 17;
                    loota.w = 100;
                    SDL_Rect sisaLoota;
                    sisaLoota.x = x-(x%64)+11;
                    sisaLoota.h = 15;
                    sisaLoota.w = 98;
                    vector<string> options = {"hakkaa puuta", "hakkaa kivestä"};
                    for (int i{0}; i < options.size(); i++){
                        loota.y = y-(y%64) + 15*i + 10;
                        sisaLoota.y = y-(y%64) + 15*i + 11;
                        SDL_RenderDrawRect(ren, &loota);
                        //muutetaan väri valkoiseksi
                        SDL_SetRenderDrawColor(ren, 0xFF, 0xFF, 0xFF, 0xFF);
                        SDL_RenderFillRect(ren, &sisaLoota);
                        //muutetaan väri takaisin mustaksi
                        SDL_SetRenderDrawColor(ren, 0x00, 0x00, 0x00, 0xFF);
                        pair<int, int> kohta{(x-x%64)/64,(y-y%64)/64};
                        options = kartta.at(kohta).getActions();
                        teksti = TTF_RenderText_Solid(font, options[i].c_str(), tekstinVari);
                        TTF_SizeUTF8(font, options[i].c_str(), &FontX, &FontY);
                        piirra(teksti, ren, x-(x%64)+(50-FontX/2) + 10, y-(y%64) + 15*i + 10, FontX, FontY);
                    }
                    //mene valikkotilaan, missä oikealla klikkauksella valitaan toiminto, eikä siirretä pelaajaa
                    valikko = true;

                    muutos = 1;

                }
                else if (SDL_GetMouseState(NULL, NULL) == SDL_BUTTON_LEFT){
                    //hakee hiiren koordinaatit
                    SDL_GetMouseState( &pelaaja_x, &pelaaja_y);
                    if (pelaaja_x >= 0 && pelaaja_x <= x_blocks*block_size && pelaaja_y >= 0 && pelaaja_y <= y_blocks*block_size){
                        //piirraTausta(piirrettavat, ren);
                        //pelaaja piirretään keskelle blockia, mitä on klikattu
                        pelaaja_x = (pelaaja_x-(pelaaja_x%64));
                        pelaaja_y = (pelaaja_y-(pelaaja_y%64));

                        //ei piirretä uudelleen, jos paikka ei ole muuttunut
                        if(pelaaja_x != edellinen_x || pelaaja_y != edellinen_y){
                            if ( edellinen_x != -1 && edellinen_y != -1){
                                piirraTausta(piirrettavat, ren, {edellinen_x/block_size, edellinen_y/block_size});
                            }
                            piirra(pelaaja, ren, pelaaja_x, pelaaja_y, 64, 64);
                            muutos = 1;
                            edellinen_x = pelaaja_x;
                            edellinen_y = pelaaja_y;
                        }
                    }
                }

            }
            if( e.type == SDL_KEYDOWN)
            {
                const Uint8 *state = SDL_GetKeyboardState(NULL);
                if( state[SDL_SCANCODE_ESCAPE]){
                    poistu = true;
                }
                //TODO: jos joku muu, kuin esc, mikä näppän? ->else ->if-lauseet, ei "else if"
                else if (state[SDL_SCANCODE_F]){
                    piirra(kirves, ren, 3, window_height-53, 50, 50);
                }
            }
        }
        //itemin piirtämisen kokeilu
        //TODO: määritä sloteille pysyvät koordinaatit, jotka tallennetaan vektoriin
        if (muutos == 1){
            piirra(kirves, ren, 3, window_height-53, 50, 50);
            SDL_RenderPresent(ren);
        }


        //Update the screen

        muutos = 0;
        SDL_Delay(50);

    }

    //odotus-funktio


    TTF_CloseFont(font);
    SDL_FreeSurface(teksti);
    SDL_FreeSurface(pelaaja);
    SDL_FreeSurface(taustaRuoho);
    SDL_FreeSurface(taustaSora);
    SDL_FreeSurface(kivi);
    SDL_FreeSurface(puu);
    SDL_FreeSurface(UI);
    SDL_FreeSurface(kirves);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    TTF_Quit();
    SDL_Quit();
    return 0;
}
