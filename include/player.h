#ifndef PLAYER_H
#define PLAYER_H

#include "item.h"
#include <map>
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <sstream>

using namespace std;

class player
{
    public:
        player(SDL_Renderer* ren);
        virtual ~player();
        //annetaan pelaajan k‰yttˆˆn tavara
        void setItem(item*);
        //alustetaan k‰ytett‰v‰t resurssit
        void initResource(int ID, string name, SDL_Texture* kuva);
        //palauttaa kysytyn resurssin varastoidun m‰‰r‰n
        int getResourceQuantity(int ID);
        //lis‰‰ tietty‰ resurssia annetun m‰‰r‰n
        void addResource(int ID, int quantity, string name="");
        //palauttaa true, jos onnistutaan ja muutoin palauttaa false
        bool useResource(int ID, int quantity);
        //piirt‰‰ resurssit UI:hin
        vector<pair<SDL_Texture*, SDL_Texture*>> drawResources();

    protected:

    private:
        struct Resurssi{
            int ID;
            string name;
            int quantity;
            SDL_Texture* kuvake;
            SDL_Texture* maaraKuva;
        };

        SDL_Color tekstinVari_;
        TTF_Font* font_;
        SDL_Renderer* ren_;

        vector<item*> tavarat_;
        //puun nimi, taidon nimi ja sen taso
        //mahdollisesti kannattaa luoda stringin tilalle oma taito luokka
        map<string, map<string, int>> taidot_;
        vector<Resurssi> resurssit_;
};

#endif // PLAYER_H
