#include "map.h"
#include "block.h"
#include "draw.h"
#include "item.h"
#include "player.h"
#include "actions.h"


/*TODO:
    -Research/skillit
    -minimap ja isompi kartta
    -painotetut blockien generoinnit
    -lis�� kohteita maastoon
    -resurssien ker��minen ja s�il�minen
    -itemien rakentaminen
*/

//constit
const int block_size = 64;
const int item_size = 50;
const int frame_size = item_size+4;
const int window_height = 1080;
const int window_width = 1920;
//lasketaan montako palikkaa mahtuu k�ytett�v�lle alueelle kent�ksi
const int x_blocks = (window_width-(window_width%block_size))/block_size;       //30 palikkaa
const int y_blocks = (window_height-(window_height%block_size))/block_size;     //16, 875


void swapToTop(int toTop, vector<int>* windowVector){
    std::cerr << windowVector->size() << " -> ";

    //jos alkio on jo oikeassa paikassa
    if(windowVector->back() == toTop){
            std::cerr << windowVector->size() << "[";
            for(auto alkio : *windowVector){
                std::cerr << alkio << ", ";
            }
            std::cerr << "] alkio valmiiksi viimeisen�" << std::endl;
        return;
    }
    //muutoin k�y l�pi vektorin, ja etsii vaihdettavaa alkiota.
    for(vector<int>::iterator it{windowVector->end()-1}; it!=windowVector->begin(); it--){
        //jos alkio l�ytyy, vaihdetaan se vektorin viimeiseksi.
        std::cerr << std::endl;
        std::cerr << *it << " <--> " << toTop << std::endl;
        if(*it == toTop){
            windowVector->push_back(*it);
            windowVector->erase(it);
    std::cerr << windowVector->size() << "[";
            for(auto alkio : *windowVector){
                std::cerr << alkio << ", ";
            }
            std::cerr << "] alkio l�ytyi, ja siirrettiin viimeiseksi" << std::endl;
            return;
        }
    }
    //jos alkiota ei ole viel� l�ydetty, lis�t��n se vektorin viimeiseksi
    windowVector->push_back(toTop);
    std::cerr << windowVector->size() << "[";
            for(auto alkio : *windowVector){
                std::cerr << alkio << ", ";
            }
            std::cerr << "] alkio lis�ttiin" << std::endl;
    return;
}

void eraseWindow(int toErase, vector<int>* windowVector){
    std::cerr << windowVector->size() << " (";
    for(vector<int>::iterator it{windowVector->end()-1}; it!=windowVector->begin(); it--){
        if(*it == toErase){
                std::cerr << *it << ") -> ";
            windowVector->erase(it);
            std::cerr << windowVector->size() << std::endl;
            return;
        }
    }
}


using namespace std;


int main(int argc, char *argv[])
{
    //annetaan piirtoluokalle vakioiden pointterit
    alustaPiirto(&block_size, &window_height, &window_width, &x_blocks, &y_blocks);

    //alustetaan SDL:n videon k�sittely, ja jos se ep�onnistuu, lopetetaan ohjelman suoritus
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
    vector<SDL_Texture*> tekstit;

    int pelaaja_x = 896;
    int pelaaja_y = 448;
    int muutos = 1;
    int edellinen_x = pelaaja_x;
    int edellinen_y = pelaaja_y;
    int valittu = 1;
    int x;
    int y;
    map<SDL_Rect*, int> valikkoMappi;
    bool poistu = false;
    bool valikko = false;
    bool mapActive = false;
    bool pauseActive = false;
    bool craftActive = false;
    enum window{game, map, pause, craft};
    vector<int>* activeWindow = new vector<int>{window::game};
    SDL_Event e;
    vector<item*> tavarat;

    //ikkunan luominen
    SDL_Window *win = SDL_CreateWindow("da Peli", 0, 0, window_width, window_height, SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE|SDL_WINDOW_INPUT_GRABBED|SDL_WINDOW_BORDERLESS);
    SDL_Window *mapWin = SDL_CreateWindow("kartta", block_size*2, block_size*2, window_width-(block_size*4), window_height-(block_size*4), SDL_WINDOW_HIDDEN|SDL_WINDOW_RESIZABLE|SDL_WINDOW_BORDERLESS);
    SDL_Window *pauseWin = SDL_CreateWindow("pause", (window_width/2)-block_size*3, (window_height/2)-(2*block_size), block_size*6, block_size*2, SDL_WINDOW_HIDDEN|SDL_WINDOW_RESIZABLE|SDL_WINDOW_BORDERLESS);
    SDL_Window *craftWin = SDL_CreateWindow("v�kerrys", block_size*2, block_size*2, window_width-(block_size*4), window_height-(block_size*4), SDL_WINDOW_HIDDEN|SDL_WINDOW_RESIZABLE|SDL_WINDOW_BORDERLESS);


    //jos ikkunan luominen ep�onnistuu
    if (win == nullptr){
        cout << "SDL_CreateWindow Error: " << SDL_GetError() << endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    SDL_Renderer *mapRen = SDL_CreateRenderer(mapWin, -1, SDL_RENDERER_ACCELERATED);
    SDL_Renderer *pauseRen = SDL_CreateRenderer(pauseWin, -1, SDL_RENDERER_ACCELERATED);
    SDL_Renderer *craftRen = SDL_CreateRenderer(craftWin, -1, SDL_RENDERER_ACCELERATED);

    player* pelaaja_ptr = new player(ren);

    //lataa kuvat
    SDL_Texture * taustaRuoho = lataaKuva(ren, "bmp/grass.bmp", false);
    SDL_Texture * taustaSora = lataaKuva(ren, "bmp/ground.bmp", false);
    SDL_Texture * vesi = lataaKuva(ren, "bmp/water.bmp", false);
    SDL_Texture * puu = lataaKuva(ren, "bmp/tree_S.bmp", true);
    SDL_Texture * kivi = lataaKuva(ren, "bmp/rock_S.bmp", true);

    SDL_Texture * pelaaja = lataaKuva(ren, "bmp/player.bmp", true);
    SDL_Texture * kirves = lataaKuva(ren, "bmp/axe.bmp", true);
    SDL_Texture * hakku = lataaKuva(ren, "bmp/pick.bmp", true);
    SDL_Texture * viikate = lataaKuva(ren, "bmp/scythe.bmp", true);
    SDL_Texture * ampari = lataaKuva(ren, "bmp/bucket.bmp", true);

    SDL_Texture * UI = lataaKuva(ren, "bmp/UI.bmp", false);
    SDL_Texture * valinta = lataaKuva(ren, "bmp/select.bmp", true);
    SDL_Texture * ruoho_resurssi = lataaKuva(ren, "bmp/grass_Res.bmp", true);
    SDL_Texture * puu_resurssi = lataaKuva(ren, "bmp/log_Res.bmp", true);
    SDL_Texture * kivi_resurssi = lataaKuva(ren, "bmp/rock_Res.bmp", true);
    SDL_Texture * vesi_resurssi = lataaKuva(ren, "bmp/water_Res.bmp", true);

    tekstinVari.r = 150;
    SDL_Surface* tempText = TTF_RenderText_Solid(font, "Quit? (Y/N)", tekstinVari);
    SDL_Texture * pauseText = SDL_CreateTextureFromSurface(pauseRen, tempText);
    SDL_FreeSurface(tempText);
    tekstinVari.r = 0;

    pelaaja_ptr->initResource(0, "ruoho", ruoho_resurssi);
    pelaaja_ptr->initResource(1, "puu", puu_resurssi);
    pelaaja_ptr->initResource(2, "kivi", kivi_resurssi);
    pelaaja_ptr->initResource(3, "vesi", vesi_resurssi);

    //luodaan vektori, jossa on kaikki k�ytett�v�t kuvat
    vector<SDL_Texture*> kuvat;
    kuvat.push_back(taustaRuoho);
    kuvat.push_back(taustaSora);
    kuvat.push_back(pelaaja);
    kuvat.push_back(puu);
    kuvat.push_back(kivi);
    kuvat.push_back(vesi);

    pair<int, int> pari;
    std::map<std::pair<int,int>, std::vector<SDL_Texture*>> piirrettavat;

    piirrettavat.insert(pair<pair<int,int>, vector<SDL_Texture*>>({-2,-2}, {UI}));

    //generoi mappi, t�ll� hetkell� randomisoi mapin
    std::map<std::pair<int,int>, block> kartta = generateMap(x_blocks, y_blocks);

    actions* actions_ptr = new actions(&block_size, &kartta, &piirrettavat, &x_blocks, &y_blocks, &kuvat, font, &tekstinVari);

    vector<SDL_Texture*> piirto;
    for (int i = 0; i<x_blocks; i++){
        for (int j = 0; j<y_blocks; j++){
            //piirt�� taustan kartta-mapin tietojen perusteella
            pair<int, int> kohta {i,j};
            piirto = maaritaPiirrettavat((kartta.at(kohta).getType()), kuvat);
            kartta.at(kohta).SetImages(piirto);
            //assosioidaan piirrett�v�t kuvat oikean blockin kanssa
            piirrettavat.insert(pair<pair<int,int>, vector<SDL_Texture*>>({i, j}, piirto));
        }
    }

    piirraTausta(piirrettavat, ren);
    piirra(pelaaja, ren, pelaaja_x, pelaaja_y, block_size, block_size);

    item axe("kirves", kirves);
    item pick("hakku", hakku);
    item scythe("viikate", viikate);
    item bucket("�mp�ri", ampari);
    tavarat.push_back(&axe);
    tavarat.push_back(&pick);
    tavarat.push_back(&scythe);
    tavarat.push_back(&bucket);

    while(poistu != true){

        //optimoitavissa threadeilla -> thread hoitamaan yksitt�isen blockin piirtoa ja vapautuessaan ottaa seuraavan k�sittelyyn

        //k�ytt�j�n sy�tteiden k�sittely
        SDL_PumpEvents();
        while ( SDL_PollEvent(&e) != 0){
            //hiiren n�pp�int� painetaan ja painettava n�pp�in on vasen
            if( e.type == SDL_MOUSEBUTTONDOWN){
                //jos painettu hiiren nappi ei ole vasen
                if(SDL_GetMouseState(NULL, NULL) != SDL_BUTTON_LEFT){
                    SDL_GetMouseState( &x, &y);
                    //jos klikkaus on kartan alueella
                    if( (x>=0) && (x<=1920) && (y>=0) && (y<=1024) ){
                        actions_ptr->showOptions(&valikkoMappi, &tekstit, ren, &x, &y, &edellinen_x, &edellinen_y);
                        //mene valikkotilaan, miss� oikealla klikkauksella valitaan toiminto, eik� siirret� pelaajaa
                        valikko = true;
                    }
                    //klikkaus tavara-alueella
                    else if( (x>=0) && (x<=1920) && (y>1024) && (y<=1080) ){
                        //UI:n right click funktionaalisuus t�nne
                    }
                    muutos = 1;
                }

                //vasen hiiren nappi eik� valikko ole aktiivisena
                else if (SDL_GetMouseState(NULL, NULL) == SDL_BUTTON_LEFT && valikko == false){
                    //hakee hiiren koordinaatit
                    SDL_GetMouseState( &pelaaja_x, &pelaaja_y);
                    //jos klikkaus oli pelialueen sis�ll�
                    if(activeWindow->back() == window::game){
                        if (pelaaja_x >= 0 && pelaaja_x <= x_blocks*block_size && pelaaja_y >= 0 && pelaaja_y <= y_blocks*block_size){
                            actions_ptr->movePlayer(ren, &pelaaja_x, &pelaaja_y, &edellinen_x, &edellinen_y);
                            muutos = 1;
                        }
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

            //n�pp�imist�n sy�tteet
            if( e.type == SDL_KEYDOWN)
            {
                //luetaan sy�te n�pp�imist�lt�
                const Uint8 *state = SDL_GetKeyboardState(NULL);
                if( state[SDL_SCANCODE_ESCAPE]){
                    if(pauseActive == false){
                        SDL_ShowWindow(pauseWin);
                        swapToTop(window::pause, activeWindow);
                        piirra(pauseText, pauseRen, block_size, block_size/2, block_size*4, block_size);
                        SDL_RenderPresent(pauseRen);
                        pauseActive = true;
                    }
                    else if(pauseActive == true){
                        SDL_HideWindow(pauseWin);
                        eraseWindow(window::pause, activeWindow);
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
                        //kartan m��rittely t�nne
                        if(mapActive == false){
                            SDL_ShowWindow(mapWin);
                            swapToTop(window::map, activeWindow);
                            mapActive = true;
                        }
                        else if(mapActive == true){
                            SDL_HideWindow(mapWin);
                            eraseWindow(window::map, activeWindow);
                            mapActive = false;
                        }
                    }

                    //research ja crafting (ehk� jollekin toiselle napille bindaus?)
                    else if(state[SDL_SCANCODE_C]){
                        //kartan m��rittely t�nne
                        if(craftActive == false){
                            SDL_ShowWindow(craftWin);
                            swapToTop(window::craft, activeWindow);
                            craftActive = true;
                        }
                        else if(craftActive == true){
                            SDL_HideWindow(craftWin);
                            eraseWindow(window::craft, activeWindow);
                            craftActive = false;
                        }
                    }

                    //toiminto valitulla ty�kalulla
                    else if(state[SDL_SCANCODE_SPACE]){
                        //toiminto laatalla valitulla ty�kalulla
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
                        //�mp�ri
                        else if(valittu == 4){
                            //jos ollaan vesi laatalla
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
                //tavara on jo poistettu. hiljennet��n virhe.
            }
        }
    }
    for(auto teksti : tekstit){
        try{
            if(teksti == NULL){throw -1;}
            else{
                SDL_DestroyTexture(teksti);
            }
        }
        catch(int e){
            if(e == -1){
                //teksti on jo poistettu. hiljennet��n virhe.
            }
        }
    }
    tekstit.clear();
    tavarat.clear();
    kartta.clear();
    delete pelaaja_ptr;
    activeWindow->clear();
    delete activeWindow;

    TTF_CloseFont(font);
    SDL_DestroyTexture(pelaaja);

    SDL_DestroyTexture(taustaRuoho);
    SDL_DestroyTexture(taustaSora);
    SDL_DestroyTexture(kivi);
    SDL_DestroyTexture(puu);
    SDL_DestroyTexture(vesi);

    SDL_DestroyTexture(UI);
    SDL_DestroyTexture(kirves);
    SDL_DestroyTexture(hakku);
    SDL_DestroyTexture(viikate);
    SDL_DestroyTexture(valinta);
    SDL_DestroyTexture(ruoho_resurssi);
    SDL_DestroyTexture(puu_resurssi);
    SDL_DestroyTexture(pauseText);

    //destroy texture

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
