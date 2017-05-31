#include "item.h"

using namespace std;

item::item(string name, SDL_Texture* kuva)
{
    name_ = name;
    kuva_ = kuva;
}

item::~item()
{

}

SDL_Texture* item::getImage(){
    return kuva_;
}

string item::getName(){
    return name_;
}
