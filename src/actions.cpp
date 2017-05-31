#include "actions.h"

actions::actions(const int* block_size_ptr, map<pair<int,int>, block>* kartta, map<pair<int,int>, vector<SDL_Texture*>>* piirrettavat,
                 const int* x_blocks, const int* y_blocks, vector<SDL_Texture*>* kuvat, TTF_Font* font, SDL_Color* tekstinVari){
    block_size_ptr_ = block_size_ptr;
    kartta_ = kartta;
    piirrettavat_ = piirrettavat;
    x_blocks_ = x_blocks;
    y_blocks_ = y_blocks;
    kuvat_ = kuvat;
    font_ = font;
    tekstinVari_ = tekstinVari;
}

actions::~actions(){
    block_size_ptr_ = NULL;
    kartta_ = NULL;
    piirrettavat_ = NULL;
    x_blocks_ = NULL;
    y_blocks_ = NULL;
    kuvat_ = NULL;
    font_ = NULL;
    tekstinVari_ = NULL;
}

void actions::showOptions(map<SDL_Rect*, int>* valikkoMappi, vector<SDL_Texture*>* tekstit, SDL_Renderer *ren,
                          int* x, int* y, int* edellinen_x, int* edellinen_y){
    //tyhjennet‰‰n edelliset valikot pois muistista
    valikkoMappi->clear();
    tekstit->clear();

    vector<string> options;
    //paikka normalisoituna palikoille, eli ID muoto
    pair<int, int> kohta{((*x)-(*x)%(*block_size_ptr_))/(*block_size_ptr_),((*y)-(*y)%(*block_size_ptr_))/(*block_size_ptr_)};
    //haetaan palikan toiminnot ID:ll‰
    options = kartta_->at(kohta).getActionsString();
    vector<int> optionsID = kartta_->at(kohta).getActionsID();
    int FontX;
    int FontY;
    std::vector<SDL_Rect*> sisaLootat;
    for (unsigned int i{0}; i < options.size(); i++){
        //m‰‰ritell‰‰n ulkolaatikon mitat
        SDL_Rect* loota = new SDL_Rect;
        loota->x = (*x)-((*x)%(*block_size_ptr_))+10;
        loota->h = 17;
        loota->w = 100;
        loota->y = (*y)-((*y)%(*block_size_ptr_)) + 16*i + 10;

        //m‰‰ritell‰‰n sis‰laatikon mitat
        SDL_Rect* sisaLoota = new SDL_Rect;
        sisaLoota->x = (*x)-((*x)%(*block_size_ptr_))+11;
        sisaLoota->h = 15;
        sisaLoota->w = 98;
        sisaLoota->y = (*y)-((*y)%(*block_size_ptr_)) + 16*i + 11;

        //peit‰ edellinen valikko
        if (edellinenValikkoX_ != -1 && edellinenValikkoY_ != -1){
            piirraTausta(*piirrettavat_, ren, {edellinenValikkoX_, edellinenValikkoY_});
            //jos valikko ei ollut reunimmaisessa palikassa, peitet‰‰n myˆs viereinen laatta varmuuden vuoksi
            if( edellinenValikkoX_ != *x_blocks_ ){piirraTausta(*piirrettavat_, ren, {edellinenValikkoX_+1, edellinenValikkoY_});}
            //piirret‰‰n pelaaja p‰‰lle
            if( *edellinen_x != -1 && *edellinen_y != -1){piirra(kuvat_->at(2), ren, *edellinen_x, *edellinen_y, 64, 64);}
        }
        //tallenetaan luotu valikkotekstilaatikko
        valikkoMappi->insert(pair<SDL_Rect*, int>(loota, optionsID.at(i)) );
        SDL_Surface* temp = TTF_RenderText_Solid(font_, options[i].c_str(), *tekstinVari_);
        tekstit->push_back(SDL_CreateTextureFromSurface(ren, temp));
        SDL_FreeSurface(temp);
        sisaLootat.push_back(sisaLoota);
    }

    int j{0};
    //piirret‰‰n luodut valikkotekstit n‰kyviin
    for(auto valinta : *valikkoMappi){
        SDL_RenderDrawRect(ren, valinta.first);
        //muutetaan v‰ri valkoiseksi
        SDL_SetRenderDrawColor(ren, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderFillRect(ren, sisaLootat.at(j));
        //muutetaan v‰ri takaisin mustaksi
        SDL_SetRenderDrawColor(ren, 0x00, 0x00, 0x00, 0xFF);
        TTF_SizeUTF8(font_, options[j].c_str(), &FontX, &FontY);
        piirra(tekstit->at(j), ren, (*x)-((*x)%(*block_size_ptr_))+(50-FontX/2) + 10, (*y)-((*y)%(*block_size_ptr_)) + 16*j + 10, FontX, FontY);
        j++;
    }
    //p‰ivitet‰‰n muistiin miss‰ viimeisin valikko on ollut
    edellinenValikkoX_ = ((*x)-(*x)%(*block_size_ptr_))/(*block_size_ptr_);
    edellinenValikkoY_ = ((*y)-(*y)%(*block_size_ptr_))/(*block_size_ptr_);
}

void actions::selectOption(map<SDL_Rect*, int>* valikkoMappi, player* pelaaja_ptr, map<pair<int,int>, block>* kartta, SDL_Renderer *ren,
                           int* x, int *y, int* edellinen_x, int* edellinen_y){
    //k‰yd‰‰n l‰pi kaikki valikon kohteet
    for(auto valinta : *valikkoMappi){
        int vertausX = (valinta.first->x - valinta.first->x%(*block_size_ptr_))/(*block_size_ptr_);
        int vertausY = (valinta.first->y - valinta.first->y%(*block_size_ptr_))/(*block_size_ptr_);

        //jos klikkaus on jonkin valikkotekstin kohdalla, tehd‰‰n sen valinnan toiminnat
        if( (*x >= valinta.first->x) && (*x <= (valinta.first->x + valinta.first->w)) &&
           (*y >= valinta.first->y) && (*y <= (valinta.first->y + valinta.first->h))){
            //tehd‰‰n valittu toiminto laatalla
            kartta->at({vertausX, vertausY}).doAction(pelaaja_ptr, valinta.second);

            //siirret‰‰n pelaaja laattaan, miss‰ toiminto suoritetaan.
            if ( *edellinen_x != -1 && *edellinen_y != -1){
                piirraTausta(*piirrettavat_, ren, {*edellinen_x/(*block_size_ptr_), *edellinen_y/(*block_size_ptr_)});
            }
            *x = *x-*x%(*block_size_ptr_);
            *y = *y-*y%(*block_size_ptr_);
            *edellinen_x = *x;
            *edellinen_y = *y;
            break;
        }
    }
    //peit‰ valikko
    piirraTausta(*piirrettavat_, ren, {edellinenValikkoX_, edellinenValikkoY_});
    //jos valikko on ollut kahdella laatalla
    if( edellinenValikkoX_ != *x_blocks_ ){piirraTausta(*piirrettavat_, ren, {edellinenValikkoX_+1, edellinenValikkoY_});}
    //piirr‰ pelaaja paikalleen
    if (*edellinen_x != -1 && *edellinen_y != -1){piirra(kuvat_->at(2), ren, *edellinen_x, *edellinen_y, *block_size_ptr_, *block_size_ptr_);}
}

void actions::movePlayer(SDL_Renderer *ren, int* pelaaja_x, int *pelaaja_y, int* edellinen_x, int* edellinen_y){
    //pelaaja piirret‰‰n keskelle blockia, mit‰ on klikattu
    *pelaaja_x = (*pelaaja_x-(*pelaaja_x%*block_size_ptr_));
    *pelaaja_y = (*pelaaja_y-(*pelaaja_y%*block_size_ptr_));

    //ei piirret‰ uudelleen, jos paikka ei ole muuttunut
    if(*pelaaja_x != *edellinen_x || *pelaaja_y != *edellinen_y){
        //peitet‰‰n pelaajan haamukuva
        if ( *edellinen_x != -1 && *edellinen_y != -1){
            piirraTausta(*piirrettavat_, ren, {*edellinen_x/(*block_size_ptr_), *edellinen_y/(*block_size_ptr_)});
        }
        //piirret‰‰n pelaaja uuteen paikkaan ja p‰ivitet‰‰n pelaajan koordinaatit muistiin
        piirra(kuvat_->at(2), ren, *pelaaja_x, *pelaaja_y, *block_size_ptr_, *block_size_ptr_);
        *edellinen_x = *pelaaja_x;
        *edellinen_y = *pelaaja_y;
    }
}
