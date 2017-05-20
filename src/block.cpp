#include "block.h"

//m‰‰rittelee luotavan palikan
block::block(int x, int y, int tempType){
    enum {ruoho, sora, puu, kivi, vesi};
    ID_ = {x,y};
    type_ = tempType;
    SetActions(type_);
    //varmistetaan ett‰ rajoitukset on alustettu tyhj‰ksi ja asetetaan ne sitten
    switch(type_){

    //ruoho
    case 0:
        limits_.clear();
        limits_ = {kivi, vesi};
        break;

    //sora
    case 1:
        limits_.clear();
        limits_ = {puu};
        break;

    //puu
    case 2:
        limits_.clear();
        limits_ = {sora, kivi, vesi};
        break;

    //kivi
    case 3:
        limits_.clear();
        limits_ = {ruoho, puu};
        break;

    //vesi
    case 4:
        limits_.clear();
        limits_ = {ruoho, puu};

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
    case 4:
        actions_.clear();
        actions_.push_back("ota vett‰");
        break;
    default:
        actions_.clear();
        actions_.push_back("tutki");
        break;
    }
}
