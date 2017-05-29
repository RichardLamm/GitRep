#include "map.h"
#include "block.h"
#include "draw.h"
#include "item.h"
#include "player.h"
#include "actions.h"


/*TODO:
    -valikko
    -oikeasti jonkinlaista interactaamista palikoiden kanssa
    -painotetut blockien generoinnit
    -lis‰‰ kohteita maastoon
    -resurssien ker‰‰minen ja s‰ilˆminen
    -itemien rakentaminen
*/

//constit
const int block_size = 64;
const int item_size = 50;
const int frame_size = item_size+4;
const int window_height = 1080;
const int window_width = 1920;
//lasketaan montako palikkaa mahtuu k‰ytett‰v‰lle alueelle kent‰ksi
const int x_blocks = (window_width-(window_width%block_size))/block_size;       //30 palikkaa
const int y_blocks = (window_height-(window_height%block_size))/block_size;     //16, 875


using namespace std;


int main(int argc, char *argv[])
{
    //annetaan piirtoluokalle vakioiden pointterit
    alustaPiirto(&block_size, &window_height, &window_width, &x_blocks, &y_blocks);

    //alustetaan SDL:n videon k‰sittely, ja jos se ep‰onnistuu, lopetetaan ohjelman suoritus
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_EVENTS) != 0){
        cout << "SDL_Init Error: " << SDL_GetError() << endl;
        return 1;
    }
    TTF_Init();
    SDL_Color tekstinVari;
    tekstinVari.b = 100;
    tekstinVari.r = 0;
    tekstinVari.g = 0;
    tekstinVari.a = 255;
    TTF_Font* font = TTF_OpenFont("ARIAL.TTF", 15);
    //TTF_SetFontOutline(font, 1);
    vector<SDL_Surface*> tekstit;

    int pelaaja_x = 896;
    int pelaaja_y = 448;
    int muutos = 1;
    int edellinen_x = pelaaja_x;
    int edellinen_y = pelaaja_y;
    int edellinenValikkoX = -1;
    int edellinenValikkoY = -1;
    int valittu = 1;
    int x;
    int y;
    map<SDL_Rect*, int> valikkoMappi;
    bool poistu = false;
    bool valikko = false;
    bool mapActive = false;
    bool pauseActive = false;
    bool craftActive = false;
    SDL_Event e;
    vector<item*> tavarat;
    player* pelaaja_ptr = new player;


    //ikkunan luominen
    SDL_Window *win = SDL_CreateWindow("da Peli", 0, 0, window_width, window_height, SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE|SDL_WINDOW_INPUT_GRABBED|SDL_WINDOW_BORDERLESS);
    SDL_Window *mapWin = SDL_CreateWindow("kartta", block_size*2, block_size*2, window_width-(block_size*4), window_height-(block_size*4), SDL_WINDOW_HIDDEN|SDL_WINDOW_RESIZABLE|SDL_WINDOW_BORDERLESS);
    SDL_Window *pauseWin = SDL_CreateWindow("pause", (window_width/2)-block_size*3, (window_height/2)-(2*block_size), block_size*6, block_size*2, SDL_WINDOW_HIDDEN|SDL_WINDOW_RESIZABLE|SDL_WINDOW_BORDERLESS);
    SDL_Window *craftWin = SDL_CreateWindow("v‰kerrys", block_size*2, block_size*2, window_width-(block_size*4), window_height-(block_size*4), SDL_WINDOW_HIDDEN|SDL_WINDOW_RESIZABLE|SDL_WINDOW_BORDERLESS);


    //jos ikkunan luominen ep‰onnistuu
    if (win == nullptr){
        cout << "SDL_CreateWindow Error: " << SDL_GetError() << endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    SDL_Renderer *mapRen = SDL_CreateRenderer(mapWin, -1, SDL_RENDERER_ACCELERATED);
    SDL_Renderer *pauseRen = SDL_CreateRenderer(pauseWin, -1, SDL_RENDERER_ACCELERATED);
    SDL_Renderer *craftRen = SDL_CreateRenderer(craftWin, -1, SDL_RENDERER_ACCELERATED);

    //lataa kuvat
    SDL_Surface * taustaRuoho = lataaKuva("bmp/grass.bmp", false);
    SDL_Surface * taustaSora = lataaKuva("bmp/ground.bmp", false);
    SDL_Surface * vesi = lataaKuva("bmp/water.bmp", false);
    SDL_Surface * puu = lataaKuva("bmp/tree_S.bmp", true);
    SDL_Surface * kivi = lataaKuva("bmp/rock_S.bmp", true);

    SDL_Surface * pelaaja = lataaKuva("bmp/player.bmp", true);
    SDL_Surface * kirves = lataaKuva("bmp/axe.bmp", true);
    SDL_Surface * hakku = lataaKuva("bmp/pick.bmp", true);
    SDL_Surface * viikate = lataaKuva("bmp/scythe.bmp", true);
    SDL_Surface * ampari = lataaKuva("bmp/bucket.bmp", true);

    SDL_Surface * UI = lataaKuva("bmp/UI.bmp", false);
    SDL_Surface * valinta = lataaKuva("bmp/select.bmp", true);
    SDL_Surface * ruoho_resurssi = lataaKuva("bmp/grass_Res.bmp", true);
    SDL_Surface * puu_resurssi = lataaKuva("bmp/log_Res.bmp", true);
    SDL_Surface * kivi_resurssi = lataaKuva("bmp/rock_Res.bmp", true);
    SDL_Surface * vesi_resurssi = lataaKuva("bmp/water_Res.bmp", true);

    tekstinVari.r = 150;
    SDL_Surface * pauseText = TTF_RenderText_Solid(font, "Quit? (Y/N)", tekstinVari);
    tekstinVari.r = 0;

    pelaaja_ptr->initResource(0, "ruoho", ruoho_resurssi);
    pelaaja_ptr->initResource(1, "puu", puu_resurssi);
    pelaaja_ptr->initResource(2, "kivi", kivi_resurssi);
    pelaaja_ptr->initResource(3, "vesi", vesi_resurssi);

    //luodaan vektori, jossa on kaikki k‰ytett‰v‰t kuvat
    vector<SDL_Surface*> kuvat;
    kuvat.push_back(taustaRuoho);
    kuvat.push_back(taustaSora);
    kuvat.push_back(pelaaja);
    kuvat.push_back(puu);
    kuvat.push_back(kivi);
    kuvat.push_back(vesi);

    pair<int, int> pari;
    map<pair<int,int>, vector<SDL_Surface*>> piirrettavat;

    piirrettavat.insert(pair<pair<int,int>, vector<SDL_Surface*>>({-2,-2}, {UI}));

    //generoi mappi, t‰ll‰ hetkell‰ randomisoi mapin
    map<pair<int,int>, block> kartta = generateMap(x_blocks, y_blocks);

    actions* actions_ptr = new actions(&block_size, &kartta, &piirrettavat, &x_blocks, &y_blocks, &kuvat, font, &tekstinVari);

    vector<SDL_Surface*> piirto;
    for (int i = 0; i<x_blocks; i++){
        for (int j = 0; j<y_blocks; j++){
            //piirt‰‰ taustan kartta-mapin tietojen perusteella
            pair<int, int> kohta {i,j};
            piirto = maaritaPiirrettavat((kartta.at(kohta).getType()), kuvat);
            kartta.at(kohta).SetImages(piirto);
            //assosioidaan piirrett‰v‰t kuvat oikean blockin kanssa
            piirrettavat.insert(pair<pair<int,int>, vector<SDL_Surface*>>({i, j}, piirto));
        }
    }

    piirraTausta(piirrettavat, ren);
    piirra(pelaaja, ren, pelaaja_x, pelaaja_y, block_size, block_size);

    item axe("kirves", kirves);
    item pick("hakku", hakku);
    item scythe("viikate", viikate);
    item bucket("‰mp‰ri", ampari);
    tavarat.push_back(&axe);
    tavarat.push_back(&pick);
    tavarat.push_back(&scythe);
    tavarat.push_back(&bucket);

    while(poistu != true){

        //optimoitavissa threadeilla -> thread hoitamaan yksitt‰isen blockin piirtoa ja vapautuessaan ottaa seuraavan k‰sittelyyn

        //k‰ytt‰j‰n syˆtteiden k‰sittely
        SDL_PumpEvents();
        while ( SDL_PollEvent(&e) != 0){
            //hiiren n‰pp‰int‰ painetaan ja painettava n‰pp‰in on vasen
            if( e.type == SDL_MOUSEBUTTONDOWN){
                //jos painettu hiiren nappi ei ole vasen
                if(SDL_GetMouseState(NULL, NULL) != SDL_BUTTON_LEFT){
                    SDL_GetMouseState( &x, &y);
                    //jos klikkaus on kartan alueella
                    if( (x>=0) && (x<=1920) && (y>=0) && (y<=1024) ){
                        actions_ptr->showOptions(&valikkoMappi, &tekstit, ren, &x, &y, &edellinen_x, &edellinen_y);
                        //mene valikkotilaan, miss‰ oikealla klikkauksella valitaan toiminto, eik‰ siirret‰ pelaajaa
                        valikko = true;
                    }
                    //klikkaus tavara-alueella
                    else if( (x>=0) && (x<=1920) && (y>1024) && (y<=1080) ){
                        //UI:n right click funktionaalisuus t‰nne
                    }
                    muutos = 1;
                }

                //vasen hiiren nappi eik‰ valikko ole aktiivisena
                else if (SDL_GetMouseState(NULL, NULL) == SDL_BUTTON_LEFT && valikko == false){
                    //hakee hiiren koordinaatit
                    SDL_GetMouseState( &pelaaja_x, &pelaaja_y);
                    //jos klikkaus oli pelialueen sis‰ll‰
                    if (pelaaja_x >= 0 && pelaaja_x <= x_blocks*block_size && pelaaja_y >= 0 && pelaaja_y <= y_blocks*block_size){
                        actions_ptr->movePlayer(ren, &pelaaja_x, &pelaaja_y, &edellinen_x, &edellinen_y);
                        muutos = 1;
                    }
                }

                //jos valikko on aktiivisena ja klikataan vasemmalla, katsotaan valittiinko jokin toiminto
                else if (SDL_GetMouseState(NULL, NULL) == SDL_BUTTON_LEFT && valikko == true){
                    SDL_GetMouseState( &x, &y);
                    actions_ptr->selectOption(&valikkoMappi, pelaaja_ptr, &kartta, ren, &x, &y, &edellinen_x, &edellinen_y);
                    valikko = false;
                    muutos = 1;
                }
            }

            //n‰pp‰imistˆn syˆtteet
            if( e.type == SDL_KEYDOWN)
            {
                const Uint8 *state = SDL_GetKeyboardState(NULL);
                if( state[SDL_SCANCODE_ESCAPE]){
                    if(pauseActive == false){
                        SDL_ShowWindow(pauseWin);
                        piirra(pauseText, pauseRen, block_size, block_size/2, block_size*4, block_size);
                        SDL_RenderPresent(pauseRen);
                        pauseActive = true;
                    }
                    else if(pauseActive == true){
                        SDL_HideWindow(pauseWin);
                        pauseActive = false;
                    }
                }
                if( state[SDL_SCANCODE_Y] && pauseActive == true){
                    poistu = true;
                }
                else if(state[SDL_SCANCODE_N] && pauseActive == true){
                    SDL_HideWindow(pauseWin);
                    pauseActive = false;
                }
                //TODO: jos joku muu, kuin esc, mik‰ n‰pp‰n? ->else ->if-lauseet, ei "else if"
                //priorisoi esc:n painamista
                else{

                    //itemit
                    if (state[SDL_SCANCODE_1]){
                        valittu = 1;
                        muutos = 1;
                    }
                    else if (state[SDL_SCANCODE_2]){
                        valittu = 2;
                        muutos = 1;
                    }
                    else if (state[SDL_SCANCODE_3]){
                        valittu = 3;
                        muutos = 1;
                    }
                    else if (state[SDL_SCANCODE_4]){
                        valittu = 4;
                        muutos = 1;
                    }

                    //kartta
                    else if(state[SDL_SCANCODE_M]){
                        //kartan m‰‰rittely t‰nne
                        if(mapActive == false){
                            SDL_ShowWindow(mapWin);
                            mapActive = true;
                        }
                        else if(mapActive == true){
                            SDL_HideWindow(mapWin);
                            mapActive = false;
                        }
                    }

                    //toiminto valitulla tyˆkalulla
                    else if(state[SDL_SCANCODE_SPACE]){
                        //toiminto laatalla valitulla tyˆkalulla
                        //kirves (jos tehty)
                        if(valittu == 1){
                            //jos ollaan puu laatalla
                            if(kartta.at({pelaaja_x/block_size, pelaaja_y/block_size}).getType() == 2){
                                kartta.at({pelaaja_x/block_size, pelaaja_y/block_size}).doAction(pelaaja_ptr, 2);
                            }
                        }
                        //hakku
                        else if(valittu == 2){
                            //jos ollaan kivi laatalla
                            if(kartta.at({pelaaja_x/block_size, pelaaja_y/block_size}).getType() == 3){
                                kartta.at({pelaaja_x/block_size, pelaaja_y/block_size}).doAction(pelaaja_ptr, 3);
                            }
                        }
                        //viikate
                        else if(valittu == 3){
                            //jos ollaan ruoho laatalla
                            if(kartta.at({pelaaja_x/block_size, pelaaja_y/block_size}).getType() == 0){
                                kartta.at({pelaaja_x/block_size, pelaaja_y/block_size}).doAction(pelaaja_ptr, 1);
                            }
                        }
                        else if(valittu == 4){
                            //jos ollaan ruoho laatalla
                            if(kartta.at({pelaaja_x/block_size, pelaaja_y/block_size}).getType() == 4){
                                kartta.at({pelaaja_x/block_size, pelaaja_y/block_size}).doAction(pelaaja_ptr, 4);
                            }
                        }
                        muutos = 1;
                    }
                }
            }
        }

        //UI piirto
        if (muutos == 1){
            int laskuri{0};
            piirraTausta(piirrettavat, ren, {-2, -2});
            piirra(valinta, ren, frame_size*(valittu-1)+(valittu+93), window_height-(frame_size+1), frame_size, frame_size);
            for(auto esine : tavarat){
                piirra(esine->getImage(), ren, (laskuri*55)+96, window_height-53, item_size, item_size);
                laskuri++;
            }
            laskuri = 0;
            for(auto resurssiKuva : pelaaja_ptr->drawResources()){
                piirra(resurssiKuva.first, ren, (laskuri*100)+350, window_height-53, 40, 40);
                piirra(resurssiKuva.second, ren, (laskuri*100)+400, window_height-53, 20, 20);
                laskuri++;
            }
            SDL_RenderPresent(ren);
        }


        //Update the screen

        muutos = 0;
        SDL_Delay(50);

    }

    //poistetaan tavarat muistista
    for(auto tavara : tavarat){
        try{
            if(tavara == NULL){throw -1;}
            else{
                delete tavara;
            }
        }
        catch(int e){
            if(e == -1){
                //tavara on jo poistettu. hiljennet‰‰n virhe.
            }
        }
    }
    for(auto teksti : tekstit){
        try{
            if(teksti == NULL){throw -1;}
            else{
                SDL_FreeSurface(teksti);
            }
        }
        catch(int e){
            if(e == -1){
                //teksti on jo poistettu. hiljennet‰‰n virhe.
            }
        }
    }
    tekstit.clear();
    tavarat.clear();
    kartta.clear();
    delete pelaaja_ptr;

    TTF_CloseFont(font);
    SDL_FreeSurface(pelaaja);

    SDL_FreeSurface(taustaRuoho);
    SDL_FreeSurface(taustaSora);
    SDL_FreeSurface(kivi);
    SDL_FreeSurface(puu);
    SDL_FreeSurface(vesi);

    SDL_FreeSurface(UI);
    SDL_FreeSurface(kirves);
    SDL_FreeSurface(hakku);
    SDL_FreeSurface(viikate);
    SDL_FreeSurface(valinta);
    SDL_FreeSurface(ruoho_resurssi);
    SDL_FreeSurface(puu_resurssi);
    SDL_FreeSurface(pauseText);

    SDL_DestroyRenderer(ren);
    SDL_DestroyRenderer(pauseRen);
    SDL_DestroyRenderer(craftRen);
    SDL_DestroyRenderer(mapRen);
    SDL_DestroyWindow(win);
    SDL_DestroyWindow(mapWin);
    SDL_DestroyWindow(craftWin);
    SDL_DestroyWindow(pauseWin);
    TTF_Quit();
    SDL_Quit();
    return 0;
}
