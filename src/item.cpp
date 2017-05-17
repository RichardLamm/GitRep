#include "item.h"

using namespace std;

item::item(string name, SDL_Surface* kuva)
{
    name_ = name;
    kuva_ = kuva;
}

item::~item()
{

}

SDL_Surface* item::getImage(){
    return kuva_;
}

string item::getName(){
    return name_;
}
