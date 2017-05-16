#include "block.h"

//m‰‰rittelee luotavan palikan
block::block(int x, int y, int tempType){
    ID_ = {x,y};
    type_ = tempType;
    SetActions(type_);
    //varmistetaan ett‰ rajoitukset on alustettu tyhj‰ksi ja asetetaan ne sitten
    switch(type_){
    case 0:
        limits_.clear();
        limits_ = {3};
        break;
    case 1:
        limits_.clear();
        limits_ = {2};
        break;
    case 2:
        limits_.clear();
        limits_ = {3};
        break;
    case 3:
        limits_.clear();
        limits_ = {0,2};
        break;
    default:
        limits_.clear();
        break;
    }
}

int block::getType(){
    return type_;
}

vector<int> block::getLimits(){
    return limits_;
}

vector<string> block::getActions(){
    return actions_;
}

void block::SetImages(vector<SDL_Surface*> images){
    kuvat_ = images;
}

void block::SetActions(int type){
    switch(type){
    case 2:
        actions_.clear();
        actions_.push_back("hakkaa puuta");
        break;
    case 3:
        actions_.clear();
        actions_.push_back("hakkaa kivest‰");
        break;
    default:
        actions_.clear();
        actions_.push_back("tutki");
        break;
    }
}
