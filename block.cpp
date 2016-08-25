#include "block.h"

block::block(int x, int y, int tempType){
    ID = {x,y};
    type = tempType;
    SetActions(type);
    switch(type){
    case 0:
        limits.clear();
        limits = {3};
        break;
    case 1:
        limits.clear();
        limits = {2};
        break;
    case 2:
        limits.clear();
        limits = {3};
        break;
    case 3:
        limits.clear();
        limits = {0,2};
        break;
    default:
        limits.clear();
        break;
    }
}

int block::getType(){
    return type;
}

vector<int> block::getLimits(vector<int> rajat){
    if (!limits.empty()){
        for( int i{0}; i<limits.size(); i++){
            rajat.push_back(limits.at(i));
        }
    }
    return rajat;
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
