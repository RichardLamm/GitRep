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
        player();
        virtual ~player();
        void setItem(item*);
        void initResource(int ID, string name, SDL_Surface* kuva);
        int getResourceQuantity(int ID);
        void addResource(int ID, int quantity, string name="");
        //palauttaa true, jos onnistutaan ja muutoin palauttaa false
        bool useResource(int ID, int quantity);
        vector<pair<SDL_Surface*, SDL_Surface*>> drawResources();

    protected:

    private:
        struct Resurssi{
            int ID;
            string name;
            int quantity;
            SDL_Surface* kuvake;
            SDL_Surface* maaraKuva;
        };

        SDL_Color tekstinVari_;
        TTF_Font* font_;

        vector<item*> tavarat_;
        //puun nimi, taidon nimi ja sen taso
        //mahdollisesti kannattaa luoda stringin tilalle oma taito luokka
        map<string, map<string, int>> taidot_;
        vector<Resurssi> resurssit_;
};

#endif // PLAYER_H
