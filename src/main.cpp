#include "map.h"
#include "block.h"
#include "draw.h"
#include "item.h"
#include "player.h"


/*TODO:
    -valikko
    -oikeasti jonkinlaista interactaamista palikoiden kanssa
    -painotetut blockien generoinnit
    -lisää kohteita maastoon
    -resurssien kerääminen ja säilöminen
    -itemien rakentaminen
*/

//constit
const int block_size = 64;
const int item_size = 50;
const int frame_size = item_size+4;
const int window_height = 1080;
const int window_width = 1920;
//lasketaan montako palikkaa mahtuu käytettävälle alueelle kentäksi
const int x_blocks = (window_width-(window_width%block_size))/block_size;       //30 palikkaa
const int y_blocks = (window_height-(window_height%block_size))/block_size;     //16, 875


using namespace std;


int main(int argc, char *argv[])
{
    //annetaan piirtoluokalle vakioiden pointterit
    alustaPiirto(&block_size, &window_height, &window_width, &x_blocks, &y_blocks);

    //alustetaan SDL:n videon käsittely, ja jos se epäonnistuu, lopetetaan ohjelman suoritus
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
    int FontX;
    int FontY;
    vector<SDL_Surface*> tekstit;

    int pelaaja_x = 896;
    int pelaaja_y = 448;
    int muutos = 1;
    int edellinen_x = pelaaja_x;
    int edellinen_y = pelaaja_y;
    int edellinenValikkoX = -1;
    int edellinenValikkoY = -1;
    int valittu = 1;
    map<SDL_Rect*, int> valikkoMappi;
    bool poistu = false;
    bool valikko = false;
    SDL_Event e;
    vector<item*> tavarat;
    player* pelaaja_ptr = new player;


    //ikkunan luominen
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
    SDL_Surface * taustaSora = lataaKuva("bmp/ground.bmp", false);
    SDL_Surface * vesi = lataaKuva("bmp/water.bmp", false);
    SDL_Surface * puu = lataaKuva("bmp/tree_S.bmp", true);
    SDL_Surface * kivi = lataaKuva("bmp/rock_S.bmp", true);

    SDL_Surface * pelaaja = lataaKuva("bmp/player.bmp", true);
    SDL_Surface * kirves = lataaKuva("bmp/axe.bmp", true);
    SDL_Surface * hakku = lataaKuva("bmp/pick.bmp", true);
    SDL_Surface * viikate = lataaKuva("bmp/scythe.bmp", true);

    SDL_Surface * UI = lataaKuva("bmp/UI.bmp", false);
    SDL_Surface * valinta = lataaKuva("bmp/select.bmp", true);
    SDL_Surface * ruoho_resurssi = lataaKuva("bmp/grass_Res.bmp", true);
    SDL_Surface * puu_resurssi = lataaKuva("bmp/log_Res.bmp", true);
    SDL_Surface * kivi_resurssi = lataaKuva("bmp/rock_Res.bmp", true);
    SDL_Surface * vesi_resurssi = lataaKuva("bmp/water_Res.bmp", true);

    pelaaja_ptr->initResource(0, "ruoho", ruoho_resurssi);
    pelaaja_ptr->initResource(1, "puu", puu_resurssi);
    pelaaja_ptr->initResource(2, "kivi", kivi_resurssi);
    pelaaja_ptr->initResource(3, "vesi", vesi_resurssi);

    //luodaan vektori, jossa on kaikki käytettävät kuvat
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
    piirra(pelaaja, ren, pelaaja_x, pelaaja_y, block_size, block_size);

    item axe("kirves", kirves);
    item pick("hakku", hakku);
    item scythe("viikate", viikate);
    tavarat.push_back(&axe);
    tavarat.push_back(&pick);
    tavarat.push_back(&scythe);

    while(poistu != true){

        //optimoitavissa threadeilla -> thread hoitamaan yksittäisen blockin piirtoa ja vapautuessaan ottaa seuraavan käsittelyyn

        //käyttäjän syötteiden käsittely
        SDL_PumpEvents();
        while ( SDL_PollEvent(&e) != 0){
            //hiiren näppäintä painetaan ja painettava näppäin on vasen
            if( e.type == SDL_MOUSEBUTTONDOWN){
                //jos painettu hiiren nappi ei ole vasen
                if(SDL_GetMouseState(NULL, NULL) != SDL_BUTTON_LEFT){
                    valikkoMappi.clear();
                    tekstit.clear();
                    int x;
                    int y;
                    SDL_GetMouseState( &x, &y);
                    //jos klikkaus on yli kartan alueen
                    if( (x<0) | (x>1920) | (y<0) | (y>1024) ){break;}
                    //klikkaus tavara-alueella
                    else if( (x>0) && (x<1920) && (y>1024) && (y<1080) ){
                        //tavaroiden right click funktionaalisuus tänne
                        break;
                    }
                    //jos klikkaus oli kartta-alueen sisällä, jatketaan normaalisti
                    SDL_Rect loota;
                    loota.x = x-(x%block_size)+10;
                    loota.h = 17;
                    loota.w = 100;
                    SDL_Rect sisaLoota;
                    sisaLoota.x = x-(x%block_size)+11;
                    sisaLoota.h = 15;
                    sisaLoota.w = 98;
                    vector<string> options;
                    //paikka normalisoituna palikoille, eli ID muoto
                    pair<int, int> kohta{(x-x%block_size)/block_size,(y-y%block_size)/block_size};
                    //haetaan palikan toiminnot ID:llä
                    options = kartta.at(kohta).getActionsString();
                    vector<int> optionsID = kartta.at(kohta).getActionsID();
                    for (unsigned int i{0}; i < options.size(); i++){
                        //peitä edellinen valikko
                        if (edellinenValikkoX != -1 && edellinenValikkoY != -1){
                            piirraTausta(piirrettavat, ren, {edellinenValikkoX, edellinenValikkoY});
                            //jos valikko ei ollut reunimmaisessa palikassa, peitetään myös viereinen laatta varmuuden vuoksi
                            if( edellinenValikkoX != x_blocks ){piirraTausta(piirrettavat, ren, {edellinenValikkoX+1, edellinenValikkoY});}
                            //piirretään pelaaja päälle
                            if( edellinen_x != -1 && edellinen_y != -1){piirra(pelaaja, ren, edellinen_x, edellinen_y, 64, 64);}
                        }
                        valikkoMappi.insert(pair<SDL_Rect*, int>(&loota, optionsID.at(i)) );
                        tekstit.push_back(TTF_RenderText_Solid(font, options[i].c_str(), tekstinVari));
                        edellinenValikkoX = (x-x%block_size)/block_size;
                        edellinenValikkoY = (y-y%block_size)/block_size;
                    }
                    int i = 0;
                    //piirretään tekstilaatikot näytölle
                    for(auto teksti : tekstit){
                        loota.y = y-(y%block_size) + 16*i + 10;
                        sisaLoota.y = y-(y%block_size) + 16*i + 11;
                        SDL_RenderDrawRect(ren, &loota);
                        //muutetaan väri valkoiseksi
                        SDL_SetRenderDrawColor(ren, 0xFF, 0xFF, 0xFF, 0xFF);
                        SDL_RenderFillRect(ren, &sisaLoota);
                        //muutetaan väri takaisin mustaksi
                        SDL_SetRenderDrawColor(ren, 0x00, 0x00, 0x00, 0xFF);
                        TTF_SizeUTF8(font, options[i].c_str(), &FontX, &FontY);
                        piirra(teksti, ren, x-(x%block_size)+(50-FontX/2) + 10, y-(y%block_size) + 16*i + 10, FontX, FontY);
                        i++;
                    }
                    //mene valikkotilaan, missä oikealla klikkauksella valitaan toiminto, eikä siirretä pelaajaa
                    valikko = true;

                    muutos = 1;
                }

                //vasen hiiren nappi eikä valikko ole aktiivisena
                else if (SDL_GetMouseState(NULL, NULL) == SDL_BUTTON_LEFT && valikko == false){
                    //hakee hiiren koordinaatit
                    SDL_GetMouseState( &pelaaja_x, &pelaaja_y);
                    if (pelaaja_x >= 0 && pelaaja_x <= x_blocks*block_size && pelaaja_y >= 0 && pelaaja_y <= y_blocks*block_size){
                        //piirraTausta(piirrettavat, ren);
                        //pelaaja piirretään keskelle blockia, mitä on klikattu
                        pelaaja_x = (pelaaja_x-(pelaaja_x%block_size));
                        pelaaja_y = (pelaaja_y-(pelaaja_y%block_size));

                        //ei piirretä uudelleen, jos paikka ei ole muuttunut
                        if(pelaaja_x != edellinen_x || pelaaja_y != edellinen_y){
                            if ( edellinen_x != -1 && edellinen_y != -1){
                                piirraTausta(piirrettavat, ren, {edellinen_x/block_size, edellinen_y/block_size});
                            }
                            piirra(pelaaja, ren, pelaaja_x, pelaaja_y, block_size, block_size);
                            muutos = 1;
                            edellinen_x = pelaaja_x;
                            edellinen_y = pelaaja_y;
                        }
                    }
                }

                //jos valikko on aktiivisena ja klikataan vasemmalla, katsotaan valittiinko jokin toiminto
                else if (SDL_GetMouseState(NULL, NULL) == SDL_BUTTON_LEFT && valikko == true){
                    int x;
                    int y;
                    SDL_GetMouseState( &x, &y);
                    x = (x-x%block_size)/block_size;
                    y = (y-y%block_size)/block_size;
                    map<SDL_Rect*, int>::iterator it = valikkoMappi.begin();
                    while (it != valikkoMappi.end()){
                        int vertausX = (it->first->x - it->first->x%block_size)/block_size;
                        int vertausY = (it->first->y - it->first->y%block_size)/block_size;
                        if((x == vertausX || x == vertausX+1) && y == vertausY ){
                            //tehdään valittu toiminto laatalla
                            kartta.at({vertausX, vertausY}).doAction(pelaaja_ptr, it->second);

                            if ( edellinen_x != -1 && edellinen_y != -1){
                                piirraTausta(piirrettavat, ren, {edellinen_x/block_size, edellinen_y/block_size});
                            }
                            pelaaja_x = vertausX*block_size;
                            pelaaja_y = vertausY*block_size;
                            piirra(pelaaja, ren, pelaaja_x, pelaaja_y, block_size, block_size);
                            muutos = 1;
                            edellinen_x = pelaaja_x;
                            edellinen_y = pelaaja_y;
                            break;
                        }
                        it++;
                    }
                    piirraTausta(piirrettavat, ren, {edellinenValikkoX, edellinenValikkoY});
                    if( edellinenValikkoX != x_blocks ){piirraTausta(piirrettavat, ren, {edellinenValikkoX+1, edellinenValikkoY});}
                    if (edellinen_x != -1 && edellinen_y != -1){piirra(pelaaja, ren, edellinen_x, edellinen_y, block_size, block_size);}
                    valikko = false;
                    muutos = 1;
                }
            }

            //näppäimistön syötteet
            if( e.type == SDL_KEYDOWN)
            {
                const Uint8 *state = SDL_GetKeyboardState(NULL);
                if( state[SDL_SCANCODE_ESCAPE]){
                    poistu = true;
                }
                //TODO: jos joku muu, kuin esc, mikä näppän? ->else ->if-lauseet, ei "else if"
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
                        //kartan määrittely tänne
                    }

                    //toiminto valitulla työkalulla
                    else if(state[SDL_SCANCODE_SPACE]){
                        //toiminto laatalla valitulla työkalulla
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
                piirra(esine->getImage(), ren, (laskuri*54)+95, window_height-53, item_size, item_size);
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
                //tavara on jo poistettu. hiljennetään virhe.
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
                //teksti on jo poistettu. hiljennetään virhe.
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

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    TTF_Quit();
    SDL_Quit();
    return 0;
}
