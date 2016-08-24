#include "map.h"
#include "block.h"
#include <iostream>
#include <time.h>
#include <stdlib.h>


std::map<std::pair<int,int>, block> generateMap(int width, int height)
{
    //mapin luominen tähän
    std::cout << "penis" << std::endl;
    srand(time(NULL));
    std::map<std::pair<int,int>, block> kartta;

    for (int i = 0; i<width; i++){
        for (int j = 0; j<height; j++){
            block loota (i, j, rand() % 4);
            kartta.insert(std::pair<std::pair<int, int>, block>({i,j}, loota));
        }
    }

    return kartta;
}
