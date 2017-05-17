#ifndef ITEM_H
#define ITEM_H

#include <iostream>
#include <vector>
#include <SDL.h>

using namespace std;

class item
{
    public:
        item(string, SDL_Surface*);
        virtual ~item();
        SDL_Surface* getImage();
        string getName();

    protected:

    private:
        string name_;
        SDL_Surface* kuva_;
        //TODO: lis‰‰ itemin statsit
};

#endif // ITEM_H
