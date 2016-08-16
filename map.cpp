#include "map.h"
#include <iostream>
#include <time.h>
#include <stdlib.h>


std::map<std::pair<int,int>, int> generateMap()
{
    //mapin luominen tähän
    std::cout << "penis" << std::endl;
    srand(time(NULL));
    std::map<std::pair<int,int>, int> kartta;

    for (int i = 0; i<10; i++){
        for (int j = 0; j<8; j++){
            kartta.insert(std::pair<std::pair<int, int>, int>({i,j}, rand() % 4));
        }
    }

    return kartta;
}
