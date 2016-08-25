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
    block(int, int, int);
    void SetImages(vector<SDL_Surface*>);
    void SetActions(int);
    vector<string> getActions();
    int getType();
    vector<int> getLimits(vector<int>);

private:
    pair<int, int> ID;
    int type;
    vector<SDL_Surface*> kuvat;
    vector<string> actions;
    vector<int> limits;
    vector<int> surround;
};

#endif // BLOCK_H_INCLUDED
