#ifndef BLOCK_H_INCLUDED
#define BLOCK_H_INCLUDED

#include <stdio.h>
#include <SDL.h>
#include <string>
#include <vector>

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
    vector<string> getActions();
    //palauttaa laatan tyypin
    int getType();
    //palauttaa laatan rajoitteet
    vector<int> getLimits();

private:
    pair<int, int> ID_;
    int type_;
    vector<SDL_Surface*> kuvat_;
    vector<string> actions_;
    vector<int> limits_;
    vector<int> surround_;
};

#endif // BLOCK_H_INCLUDED
