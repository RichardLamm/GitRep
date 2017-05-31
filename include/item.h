#ifndef ITEM_H
#define ITEM_H

#include <iostream>
#include <vector>
#include <SDL.h>

using namespace std;

class item
{
    public:
        item(string, SDL_Texture*);
        virtual ~item();
        SDL_Texture* getImage();
        string getName();

    protected:

    private:
        string name_;
        SDL_Texture* kuva_;
        //TODO: lis‰‰ itemin statsit
};

#endif // ITEM_H
