#include "player.h"

player::player(){

}

player::~player(){

}

void player::setItem(item* tavara){
    //ei lis�t�, jos tavara on jo pelaajalla
    for (auto pelaajalla: tavarat_){
        if (pelaajalla->getName() == tavara->getName()){
            return;
        }
    }
    tavarat_.push_back(tavara);
    return;
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
    //jos resurssi on jo olemassa, lis�t��n m��r��
    for(auto asia: resurssit_){
        if (asia.ID == ID){
            asia.quantity += quantity;
            return;
        }
    }
    //luodaan resurssi, jos sit� ei viel� ole
    Resurssi resurssi;
    resurssi.ID = ID;
    resurssi.name = name;
    resurssi.quantity = quantity;
    resurssit_.push_back(resurssi);
    return;
}

bool player::useResource(int ID, int quantity){
    //k�ytet��n resurssia, jos sellainen l�ytyy
    for(auto asia: resurssit_){
        if (asia.ID == ID){
            //jos resurssia on k�ytett�viss� tarvittava m��r�
            if((asia.quantity - quantity) >= 0){
                asia.quantity -= quantity;
                return true;
            }
        }
    }
    //jos resurssin k�ytt�minen ep�onnistuu, palautetaan false
    return false;
}
