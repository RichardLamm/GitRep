#include "map.h"
#include "block.h"
#include <iostream>
#include <time.h>
#include <stdlib.h>


const int Tiles = 4;

std::map<std::pair<int,int>, block> generateMap(int width, int height)
{
    //alustetaan randomisaatio
    srand(time(NULL));
    std::map<std::pair<int,int>, block> kartta;
    vector<int> rajotteet{};
    //pit‰‰ tallessa k‰ytett‰v‰t laattatyypit
    vector<int> laatat{};
    //k‰ytett‰viss‰ olevat laattatyypit
    for (int i{0}; i<Tiles; i++){
        laatat.push_back(i);
    }
    //vektoriin karsitaan vain k‰ytˆss‰ olevat laatat arvontaa varten
    vector<int> arvottavat;

    //k‰yd‰‰n kaikki k‰ytett‰v‰t koordinaatit l‰pi ja luodaan niille palikka

    //k‰yd‰‰n l‰pi pystysuunta
    for (int j = 0; j<height; j++){
        //k‰yd‰‰n l‰pi vaakasuunta
        for (int i = 0; i<width; i++){
            arvottavat = laatat;
            rajotteet.clear();
            if ( i-1 >= 0 ){rajotteet = kartta.at({i-1, j}).getLimits();}
            if ( j-1 >= 0 ){rajotteet = kartta.at({i, j-1}).getLimits();}
            //k‰yd‰‰n l‰pi rajoitteet
            for (unsigned int k{0}; k<rajotteet.size(); k++){
                //k‰yd‰‰n l‰pi k‰ytett‰v‰t laatat
                for (unsigned int n{0}; n<arvottavat.size(); n++){
                    //etsit‰‰n kielletty laatta arvottavista ja estet‰‰n sen k‰yttˆ arvonnassa
                    if (arvottavat.at(n) == rajotteet.at(k)){
                        arvottavat.erase(arvottavat.begin() + n);
                        //siirryt‰‰n seuraavaan rajoitteeseen
                        break;
                    }
                }
            }
            //j‰ljell‰ on vain sallitut laatat ja arvotaan niist‰ jokin
            int arpa = rand() % arvottavat.size();
            //luodaan palikka koordinaatti id:ll‰ ja arvotulla tyypill‰
            block loota (i, j, arvottavat.at(arpa) );
            //lis‰t‰‰n luotu palikka karttaan
            kartta.insert(std::pair<std::pair<int, int>, block>({i,j}, loota));
        }
    }
    return kartta;
}
