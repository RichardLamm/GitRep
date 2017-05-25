#ifndef BLOCK_H_INCLUDED
#define BLOCK_H_INCLUDED

#include <stdio.h>
#include <SDL.h>
#include <string>
#include <vector>
#include "player.h"

using namespace std;

//luokka kaikille blockeille
class block{

public:
    //x, y, tyyppi
    block(int, int, int);
    void SetImages(vector<SDL_Surface*>);
    //toiminnat tietylle laattatyypille
    void SetActions(int);
    //palauttaa laatan toiminnat
    vector<string> getActionsString();
    //palauttaa toiminnan ID:n. nopeempi vertailu, kuin stringill‰
    vector<int> getActionsID();
    //palauttaa laatan tyypin
    int getType();
    //palauttaa laatan rajoitteet
    vector<int> getLimits();
    //tehd‰‰n palikassa toiminto
    void doAction(player*, int);

private:
    pair<int, int> ID_;
    int type_;
    vector<SDL_Surface*> kuvat_;
    vector<string> actionsString_;
    vector<int> actionsID_;
    vector<int> limits_;
    vector<int> surround_;
};

#endif // BLOCK_H_INCLUDED
