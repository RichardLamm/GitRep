#include "map.h"
#include "block.h"
#include <iostream>
#include <time.h>
#include <stdlib.h>


const int Tiles = 4;

std::map<std::pair<int,int>, block> generateMap(int width, int height)
{
    //mapin luominen tähän
    std::cout << "penis" << std::endl;
    srand(time(NULL));
    std::map<std::pair<int,int>, block> kartta;
    vector<int> rajotteet;
    vector<int> laatat;
    laatat.clear();
    for (int i{0}; i<Tiles; i++){
        laatat.push_back(i);
    }
    vector<int> arvottavat;

    for (int j = 0; j<height; j++){
        for (int i = 0; i<width; i++){
            arvottavat = laatat;
            rajotteet.clear();
            if ( i-1 >= 0 ){rajotteet = kartta.at({i-1, j}).getLimits(rajotteet);}
            if ( j-1 >= 0 ){rajotteet = kartta.at({i, j-1}).getLimits(rajotteet);}
            for ( int k{0}; k<rajotteet.size(); k++){
                for ( int n{0}; n<arvottavat.size(); n++){
                    if (arvottavat.at(n) == rajotteet.at(k)){
                        arvottavat.erase(arvottavat.begin() + n);
                        break;
                    }
                }
            }
            int arpa = rand() % arvottavat.size();
            block loota (i, j, arvottavat.at(arpa) );
            kartta.insert(std::pair<std::pair<int, int>, block>({i,j}, loota));
        }
    }

    return kartta;
}
