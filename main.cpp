#include "map.h"
#include "block.h"
#include "draw.h"


/*TODO:
    -valikko
    -yleinen p‰‰llepiirto -> ei j‰‰ "haamukuvia"
    -piirtoj‰rjestys mappiin (depth) ja uudenlainen piirtofunktio?
    -oikeasti jonkinlaista interactaamista palikoiden kanssa
    -kartan laajempi generoiminen
    -painotetut blockien generoinnit
    -pelaajan modelin muuttaminen
    -lis‰‰ kohteita maastoon
*/

//constit
const int block_size = 64;
const int window_height = 1080;
const int window_width = 1920;
//lasketaan montako palikkaa mahtuu k‰ytett‰v‰lle alueelle kent‰ksi
const int x_blocks = (window_width-(window_width%block_size))/block_size;       //30 palikkaa
const int y_blocks = (window_height-(window_height%block_size))/block_size;     //16,875


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
    int FontX;
    int FontY;
    SDL_Surface* teksti;

    int pelaaja_x = 896;
    int pelaaja_y = 448;
    int muutos = 1;
    int edellinen_x = pelaaja_x;
    int edellinen_y = pelaaja_y;
    int edellinenValikkoX = -1;
    int edellinenValikkoY = -1;
    map<SDL_Rect*,string> valikkoMappi;
    bool poistu = false;
    bool valikko = false;
    SDL_Event e;

    SDL_Window *win = SDL_CreateWindow("da Peli", 0, 0, window_width, window_height, SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE|SDL_WINDOW_INPUT_GRABBED|SDL_WINDOW_BORDERLESS);
    //jos ikkunan luominen ep‰onnistuu
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


    //luodaan vektori, jossa on kaikki k‰ytett‰v‰t kuvat
    vector<SDL_Surface*> kuvat;
    kuvat.push_back(taustaRuoho);
    kuvat.push_back(taustaSora);
    kuvat.push_back(pelaaja);
    kuvat.push_back(puu);
    kuvat.push_back(kivi);

    pair<int, int> pari;
    map<pair<int,int>, vector<SDL_Surface*>> piirrettavat;

    piirrettavat.insert(pair<pair<int,int>, vector<SDL_Surface*>>({-1,-1}, {UI}));

    //generoi mappi, t‰ll‰ hetkell‰ randomisoi mapin
    map<pair<int,int>, block> kartta = generateMap(x_blocks, y_blocks);

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


    while(poistu != true){

        //optimoitavissa threadeilla -> thread hoitamaan yksitt‰isen blockin piirtoa ja vapautuessaan ottaa seuraavan k‰sittelyyn

        //k‰ytt‰j‰n syˆtteiden k‰sittely
        SDL_PumpEvents();
        while ( SDL_PollEvent(&e) != 0){
            //hiiren n‰pp‰int‰ painetaan ja painettava n‰pp‰in on vasen
            if( e.type == SDL_MOUSEBUTTONDOWN){
                //jos painettu hiiren nappi ei ole vasen
                if(SDL_GetMouseState(NULL, NULL) != SDL_BUTTON_LEFT){
                    valikkoMappi.clear();
                    int x;
                    int y;
                    SDL_GetMouseState( &x, &y);
                    //jos klikkaus on yli kartan alueen
                    if( (x<0) | (x>1920) | (y<0) | (y>1024) ){break;}
                    //klikkaus tavara-alueella
                    else if( (x>0) && (x<1920) && (y>1024) && (y<1080) ){
                        //tavaroiden right click funktionaalisuus t‰nne
                        break;
                    }
                    //jos klikkaus oli kartta-alueen sis‰ll‰, jatketaan normaalisti
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
                    //haetaan palikan toiminnot ID:ll‰
                    options = kartta.at(kohta).getActions();
                    for (unsigned int i{0}; i < options.size(); i++){
                        loota.y = y-(y%block_size) + 15*i + 10;
                        sisaLoota.y = y-(y%block_size) + 15*i + 11;
                        if (edellinenValikkoX != -1 && edellinenValikkoY != -1){
                            piirraTausta(piirrettavat, ren, {edellinenValikkoX, edellinenValikkoY});
                            if( edellinenValikkoX != x_blocks ){piirraTausta(piirrettavat, ren, {edellinenValikkoX+1, edellinenValikkoY});}
                            if (edellinen_x != -1 && edellinen_y != -1){piirra(pelaaja, ren, edellinen_x, edellinen_y, 64, 64);}
                        }

                        valikkoMappi.insert(pair<SDL_Rect*, string>(&loota, options.at(i).substr()) );
                        SDL_RenderDrawRect(ren, &loota);
                        //muutetaan v‰ri valkoiseksi
                        SDL_SetRenderDrawColor(ren, 0xFF, 0xFF, 0xFF, 0xFF);
                        SDL_RenderFillRect(ren, &sisaLoota);
                        //muutetaan v‰ri takaisin mustaksi
                        SDL_SetRenderDrawColor(ren, 0x00, 0x00, 0x00, 0xFF);
                        teksti = TTF_RenderText_Solid(font, options[i].c_str(), tekstinVari);
                        TTF_SizeUTF8(font, options[i].c_str(), &FontX, &FontY);
                        piirra(teksti, ren, x-(x%block_size)+(50-FontX/2) + 10, y-(y%block_size) + 15*i + 10, FontX, FontY);
                        edellinenValikkoX = (x-x%block_size)/block_size;
                        edellinenValikkoY = (y-y%block_size)/block_size;
                    }
                    //mene valikkotilaan, miss‰ oikealla klikkauksella valitaan toiminto, eik‰ siirret‰ pelaajaa
                    valikko = true;

                    muutos = 1;

                }
                else if (SDL_GetMouseState(NULL, NULL) == SDL_BUTTON_LEFT && valikko == false){
                    //hakee hiiren koordinaatit
                    SDL_GetMouseState( &pelaaja_x, &pelaaja_y);
                    if (pelaaja_x >= 0 && pelaaja_x <= x_blocks*block_size && pelaaja_y >= 0 && pelaaja_y <= y_blocks*block_size){
                        //piirraTausta(piirrettavat, ren);
                        //pelaaja piirret‰‰n keskelle blockia, mit‰ on klikattu
                        pelaaja_x = (pelaaja_x-(pelaaja_x%block_size));
                        pelaaja_y = (pelaaja_y-(pelaaja_y%block_size));

                        //ei piirret‰ uudelleen, jos paikka ei ole muuttunut
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
                    map<SDL_Rect*, string>::iterator it = valikkoMappi.begin();
                    while (it != valikkoMappi.end()){
                        int vertausX = (it->first->x - it->first->x%block_size)/block_size;
                        int vertausY = (it->first->y - it->first->y%block_size)/block_size;
                        if((x == vertausX || x == vertausX+1) && y == vertausY ){
                            string valinta = it->second;
                            //funktio, jolle argumenttina string valinta ja joka prosessoi kyseisen valinnan
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
            //n‰pp‰imistˆn syˆtteet
            if( e.type == SDL_KEYDOWN)
            {
                const Uint8 *state = SDL_GetKeyboardState(NULL);
                if( state[SDL_SCANCODE_ESCAPE]){
                    poistu = true;
                }
                //TODO: jos joku muu, kuin esc, mik‰ n‰pp‰n? ->else ->if-lauseet, ei "else if"
                else if (state[SDL_SCANCODE_F]){
                    piirra(kirves, ren, 3, window_height-53, 50, 50);
                }
            }
        }
        //itemin piirt‰misen kokeilu
        //TODO: m‰‰rit‰ sloteille pysyv‰t koordinaatit, jotka tallennetaan vektoriin
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
