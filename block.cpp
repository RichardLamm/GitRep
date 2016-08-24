#include "block.h"

block::block(int x, int y, int tempType){
    ID = {x,y};
    type = tempType;
    SetActions(type);
}

int block::getType(){
    return type;
}

vector<string> block::getActions(){
    return actions;
}

void block::SetImages(vector<SDL_Surface*> images){
    kuvat = images;
}

void block::SetActions(int type){
    switch(type){
    case 2:
        actions.clear();
        actions.push_back("hakkaa puuta");
        break;
    case 3:
        actions.clear();
        actions.push_back("hakkaa kivestä");
        break;
    default:
        actions.clear();
        actions.push_back("tutki");
        break;
    }
}
