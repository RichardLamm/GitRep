#ifndef PLAYER_H
#define PLAYER_H

#include "item.h"
#include <map>

using namespace std;

class player
{
    public:
        player();
        virtual ~player();
        void setItem(item*);
        int getResourceQuantity(int ID);
        void addResource(int ID, int quantity, string name="");
        //palauttaa true, jos onnistutaan ja muutoin palauttaa false
        bool useResource(int ID, int quantity);

    protected:

    private:
        struct Resurssi{
            int ID;
            string name;
            int quantity{0};
        };

        vector<item*> tavarat_;
        //puun nimi, taidon nimi ja sen taso
        //mahdollisesti kannattaa luoda stringin tilalle oma taito luokka
        map<string, map<string, int>> taidot_;
        vector<Resurssi> resurssit_;
};

#endif // PLAYER_H
