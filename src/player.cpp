#include "player.h"

player::player(){
    tekstinVari_.b = 100;
    tekstinVari_.r = 0;
    tekstinVari_.g = 0;
    tekstinVari_.a = 255;
    font_ = TTF_OpenFont("ARIAL.TTF", 15);
}

player::~player(){
    font_ = NULL;
    tavarat_.clear();
    taidot_.clear();
    resurssit_.clear();
}

void player::setItem(item* tavara){
    //ei lisätä, jos tavara on jo pelaajalla
    for (auto pelaajalla: tavarat_){
        if (pelaajalla->getName() == tavara->getName()){
            return;
        }
    }
    tavarat_.push_back(tavara);
    return;
}

void player::initResource(int ID, string name, SDL_Surface* kuva){
    Resurssi resurssi;
    resurssi.ID = ID;
    resurssi.name = name;
    resurssi.kuvake = kuva;
    resurssi.quantity = 0;
    stringstream lukuStream;
    lukuStream << resurssi.quantity;
    string luku = lukuStream.str();
    resurssi.maaraKuva = TTF_RenderText_Solid(font_, luku.c_str(), tekstinVari_);
    resurssit_.push_back(resurssi);
}

int player::getResourceQuantity(int ID){
    for(auto asia: resurssit_){
        if (asia.ID == ID){
            return asia.quantity;
        }
    }
    return 0;
}

void player::addResource(int ID, int quantity, string name){
    //jos resurssi on jo olemassa, lisätään määrää
    vector<Resurssi>::iterator it = resurssit_.begin();
    for(;it != resurssit_.end(); it++){
        if (it->ID == ID){
            it->quantity += quantity;
            stringstream lukuStream;
            lukuStream << it->quantity;
            string luku = lukuStream.str();
            it->maaraKuva = TTF_RenderText_Solid(font_, luku.c_str(), tekstinVari_);
            return;
        }
    }
    //luodaan resurssi, jos sitä ei vielä ole
    Resurssi resurssi;
    resurssi.ID = ID;
    resurssi.name = name;
    resurssi.quantity = quantity;
    resurssit_.push_back(resurssi);
    return;
}

bool player::useResource(int ID, int quantity){
    //käytetään resurssia, jos sellainen löytyy
    vector<Resurssi>::iterator it = resurssit_.begin();
    for(;it != resurssit_.end(); it++){
        if (it->ID == ID){
            if(it->quantity - quantity >= 0){
                it->quantity -= quantity;
                stringstream lukuStream;
                lukuStream << it->quantity;
                string luku = lukuStream.str();
                it->maaraKuva = TTF_RenderText_Solid(font_, luku.c_str(), tekstinVari_);
                return true;
            }
        }
    }
    //jos resurssin käyttäminen epäonnistuu, palautetaan false
    return false;
}

vector<pair<SDL_Surface*, SDL_Surface*>> player::drawResources(){
    vector<pair<SDL_Surface*, SDL_Surface*>> resursssiKuvat;
    for(auto resurssi : resurssit_){
        if(resurssi.kuvake != NULL && resurssi.maaraKuva != NULL){
            resursssiKuvat.push_back({resurssi.kuvake, resurssi.maaraKuva});
        }
    }
    return resursssiKuvat;
}
