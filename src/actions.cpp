#include "actions.h"

actions::actions(const int* block_size_ptr, map<pair<int,int>, block>* kartta, map<pair<int,int>, vector<SDL_Surface*>>* piirrettavat,
                 const int* x_blocks, const int* y_blocks, vector<SDL_Surface*>* kuvat, TTF_Font* font, SDL_Color* tekstinVari){
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

}

void actions::showOptions(map<SDL_Rect*, int>* valikkoMappi, vector<SDL_Surface*>* tekstit, SDL_Renderer *ren,
                          int* x, int* y, int* edellinen_x, int* edellinen_y){
    valikkoMappi->clear();
    tekstit->clear();

    vector<string> options;
    //paikka normalisoituna palikoille, eli ID muoto
    pair<int, int> kohta{((*x)-(*x)%(*block_size_ptr_))/(*block_size_ptr_),((*y)-(*y)%(*block_size_ptr_))/(*block_size_ptr_)};
    //haetaan palikan toiminnot ID:llä
    options = kartta_->at(kohta).getActionsString();
    vector<int> optionsID = kartta_->at(kohta).getActionsID();
    int FontX;
    int FontY;
    std::vector<SDL_Rect*> sisaLootat;
    for (unsigned int i{0}; i < options.size(); i++){
        SDL_Rect* loota = new SDL_Rect;
        SDL_Rect* sisaLoota = new SDL_Rect;
        loota->x = (*x)-((*x)%(*block_size_ptr_))+10;
        loota->h = 17;
        loota->w = 100;
        sisaLoota->x = (*x)-((*x)%(*block_size_ptr_))+11;
        sisaLoota->h = 15;
        sisaLoota->w = 98;
        loota->y = (*y)-((*y)%(*block_size_ptr_)) + 16*i + 10;
        sisaLoota->y = (*y)-((*y)%(*block_size_ptr_)) + 16*i + 11;
        //peitä edellinen valikko
        if (edellinenValikkoX_ != -1 && edellinenValikkoY_ != -1){
            piirraTausta(*piirrettavat_, ren, {edellinenValikkoX_, edellinenValikkoY_});
            //jos valikko ei ollut reunimmaisessa palikassa, peitetään myös viereinen laatta varmuuden vuoksi
            if( edellinenValikkoX_ != *x_blocks_ ){piirraTausta(*piirrettavat_, ren, {edellinenValikkoX_+1, edellinenValikkoY_});}
            //piirretään pelaaja päälle
            if( *edellinen_x != -1 && *edellinen_y != -1){piirra(kuvat_->at(2), ren, *edellinen_x, *edellinen_y, 64, 64);}
        }
        valikkoMappi->insert(pair<SDL_Rect*, int>(loota, optionsID.at(i)) );
        tekstit->push_back(TTF_RenderText_Solid(font_, options[i].c_str(), *tekstinVari_));
        edellinenValikkoX_ = ((*x)-(*x)%(*block_size_ptr_))/(*block_size_ptr_);
        edellinenValikkoY_ = ((*y)-(*y)%(*block_size_ptr_))/(*block_size_ptr_);
        sisaLootat.push_back(sisaLoota);

    }
    int j{0};
    for(auto valinta : *valikkoMappi){
        SDL_RenderDrawRect(ren, valinta.first);
        //muutetaan väri valkoiseksi
        SDL_SetRenderDrawColor(ren, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderFillRect(ren, sisaLootat.at(j));
        //muutetaan väri takaisin mustaksi
        SDL_SetRenderDrawColor(ren, 0x00, 0x00, 0x00, 0xFF);
        TTF_SizeUTF8(font_, options[j].c_str(), &FontX, &FontY);
        piirra(tekstit->at(j), ren, (*x)-((*x)%(*block_size_ptr_))+(50-FontX/2) + 10, (*y)-((*y)%(*block_size_ptr_)) + 16*j + 10, FontX, FontY);
        j++;
    }
    std::cerr << " -> "  << valikkoMappi->size() <<std::endl;
}

void actions::selectOption(map<SDL_Rect*, int>* valikkoMappi, player* pelaaja_ptr, map<pair<int,int>, block>* kartta, SDL_Renderer *ren,
                           int* x, int *y, int* edellinen_x, int* edellinen_y){
    for(auto valinta : *valikkoMappi){
        int vertausX = (valinta.first->x - valinta.first->x%(*block_size_ptr_))/(*block_size_ptr_);
        int vertausY = (valinta.first->y - valinta.first->y%(*block_size_ptr_))/(*block_size_ptr_);

        std::cerr << *x << " - [" << valinta.first->x << ", " << valinta.first->x + valinta.first->w << "]" << std::endl;
        std::cerr << *y << " - [" << valinta.first->y << ", " << valinta.first->y + valinta.first->h << "]" << std::endl;

        if( (*x >= valinta.first->x) && (*x <= (valinta.first->x + valinta.first->w)) &&
           (*y >= valinta.first->y) && (*y <= (valinta.first->y + valinta.first->h))){
            //tehdään valittu toiminto laatalla
            kartta->at({vertausX, vertausY}).doAction(pelaaja_ptr, valinta.second);

            if ( *edellinen_x != -1 && *edellinen_y != -1){
                piirraTausta(*piirrettavat_, ren, {*edellinen_x/(*block_size_ptr_), *edellinen_y/(*block_size_ptr_)});
            }
            *x = *x-*x%(*block_size_ptr_);
            *y = *y-*y%(*block_size_ptr_);
            piirra(kuvat_->at(2), ren, *x, *y, *block_size_ptr_, *block_size_ptr_);
            *edellinen_x = *x;
            *edellinen_y = *y;
            break;
        }
    }
    piirraTausta(*piirrettavat_, ren, {edellinenValikkoX_, edellinenValikkoY_});
    if( edellinenValikkoX_ != *x_blocks_ ){piirraTausta(*piirrettavat_, ren, {edellinenValikkoX_+1, edellinenValikkoY_});}
    if (*edellinen_x != -1 && *edellinen_y != -1){piirra(kuvat_->at(2), ren, *edellinen_x, *edellinen_y, *block_size_ptr_, *block_size_ptr_);}
}

void actions::movePlayer(SDL_Renderer *ren, int* pelaaja_x, int *pelaaja_y, int* edellinen_x, int* edellinen_y){
    //pelaaja piirretään keskelle blockia, mitä on klikattu
    *pelaaja_x = (*pelaaja_x-(*pelaaja_x%*block_size_ptr_));
    *pelaaja_y = (*pelaaja_y-(*pelaaja_y%*block_size_ptr_));

    //ei piirretä uudelleen, jos paikka ei ole muuttunut
    if(*pelaaja_x != *edellinen_x || *pelaaja_y != *edellinen_y){
        if ( *edellinen_x != -1 && *edellinen_y != -1){
            piirraTausta(*piirrettavat_, ren, {*edellinen_x/(*block_size_ptr_), *edellinen_y/(*block_size_ptr_)});
        }
        piirra(kuvat_->at(2), ren, *pelaaja_x, *pelaaja_y, *block_size_ptr_, *block_size_ptr_);
        *edellinen_x = *pelaaja_x;
        *edellinen_y = *pelaaja_y;
    }
}
