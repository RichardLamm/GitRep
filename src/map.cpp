#include "map.h"
#include "block.h"
#include <iostream>
#include <time.h>
#include <stdlib.h>


const int Tiles = 5;

//kartan generointi
std::map<std::pair<int,int>, block> generateMap(int width, int height)
{
    //TODO: palikoiden arvonnalle painotus

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
    int arpa{0};

    //k‰yd‰‰n kaikki k‰ytett‰v‰t koordinaatit l‰pi ja luodaan niille palikka

    //k‰yd‰‰n l‰pi pystysuunta
    for (int j = 0; j<height; j++){
        //k‰yd‰‰n l‰pi vaakasuunta
        for (int i = 0; i<width; i++){
            //palautetaan kaikki laatat arvottavien joukkoon
            arvottavat = laatat;
            //nollataan rajoitteet
            rajotteet.clear();

            std::cerr << i << "," << j << " rajoitteena olivat: ";
            //haetaan uudet rajoitteet, jos sellaisia on
            //rajoite vasemmalta
            if ( i-1 >= 0 ){
                std::cerr << "i-puolella: ";
                for(auto iRajoite : kartta.at({i-1, j}).getLimits()){
                    rajotteet.push_back(iRajoite);
                    std::cerr << iRajoite << ", ";
                }
            }
            //rajoite ylh‰‰lt‰
            if ( j-1 >= 0 ){
                std::cerr << "j-puolella: ";
                for(auto jRajoite : kartta.at({i, j-1}).getLimits()){
                    rajotteet.push_back(jRajoite);
                    std::cerr << jRajoite << ", ";
                }
            }
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

            //est‰‰ tilanteet, joissa j‰‰d‰‰n jumiin kartan luonnissa.
            //lopullinen ratkaisu luoda tarpeeksi laattoja, ett‰ saadaan j‰rkev‰ generointi luonnostaan.
            if(arvottavat.size() == 0){
                arvottavat = laatat;
                arpa = 1;

            }
            else{
                arpa = rand() % arvottavat.size();
            }
            //luodaan palikka koordinaatti id:ll‰ ja arvotulla tyypill‰
            block loota (i, j, arvottavat.at(arpa) );



            std::cerr << "valittiin " << arvottavat.at(arpa) << std::endl;

            //lis‰t‰‰n luotu palikka karttaan
            kartta.insert(std::pair<std::pair<int, int>, block>({i,j}, loota));
        }
    }
    return kartta;
}
