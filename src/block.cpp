#include "block.h"

#include <iostream>

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
        break;

    default:
        limits_.clear();
        break;
    }
}

block::~block(){
    ID_ = {0,0};
    type_ = 0;
    kuvat_.clear();
    actionsID_.clear();
    actionsString_.clear();
    limits_.clear();
    surround_.clear();
}

int block::getType(){
    return type_;
}

vector<int> block::getLimits(){
    return limits_;
}

vector<string> block::getActionsString(){
    return actionsString_;
}

vector<int> block::getActionsID(){
    return actionsID_;
}

void block::SetImages(vector<SDL_Surface*> images){
    kuvat_ = images;
}

void block::SetActions(int type){
    /*
    toiminnot:
        0 = tutki
        1 = ker‰‰ ruohoa
        2 = hakkaa puuta
        3 = hakkaa kivest‰
        4 = ota vett‰

    laatat:
        0 = ruoho
        1 = sora
        2 = puu
        3 = kivi
        4 = vesi
    */
    switch(type){
    case 0:
        actionsString_.clear();
        actionsString_.push_back("ker‰‰ ruohoa");
        actionsString_.push_back("tutki");
        actionsID_.clear();
        actionsID_.push_back(1);
        actionsID_.push_back(0);
        break;
    case 2:
        actionsString_.clear();
        actionsString_.push_back("hakkaa puuta");
        actionsString_.push_back("tutki");
        actionsID_.clear();
        actionsID_.push_back(2);
        actionsID_.push_back(0);
        break;
    case 3:
        actionsString_.clear();
        actionsString_.push_back("hakkaa kivest‰");
        actionsString_.push_back("tutki");
        actionsID_.clear();
        actionsID_.push_back(3);
        actionsID_.push_back(0);
        break;
    case 4:
        actionsString_.clear();
        actionsString_.push_back("ota vett‰");
        actionsString_.push_back("tutki");
        actionsID_.clear();
        actionsID_.push_back(4);
        actionsID_.push_back(0);
        break;
    default:
        actionsString_.clear();
        actionsString_.push_back("tutki");
        actionsID_.clear();
        actionsID_.push_back(0);
        break;
    }
}

void block::doAction(player* pelaaja, int action){
    /*
    toiminnot:
        0 = tutki
        1 = ker‰‰ ruohoa
        2 = hakkaa puuta
        3 = hakkaa kivest‰
        4 = ota vett‰

    laatat:
        0 = ruoho
        1 = sora
        2 = puu
        3 = kivi
        4 = vesi
    */
    switch(action){
    case 0:
        //anna pelaajalle jotakin infoa laatasta
        break;
    case 1:
        if(type_ == 0){
            pelaaja->addResource(0, 5, "Ruoho");
        }
        break;
    case 2:
        if(type_ == 2){
            pelaaja->addResource(1, 10, "puu");
        }
        break;
    case 3:
       if(type_ == 3){
            pelaaja->addResource(2, 5, "kivi");
        }
        break;
    case 4:
        if(type_ == 4){
            pelaaja->addResource(3, 1, "vesi");
        }
        break;
    default:
        //jotakin pieless‰
        std::cerr << "toiminna suorittamisessa meni jokin pieleen" << std::endl;
        break;
    }
}
