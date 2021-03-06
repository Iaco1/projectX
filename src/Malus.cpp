#include "Malus.hpp"

Malus::Malus(){}

Malus::Malus(int x, int y, int hp, int dp, MalusType mt, int quantity) : Item(x,y, TileType::MALUS, hp, dp){
    this->mt = mt;
    this->quantity = quantity;
}

//on-screen representation for each MalusType
char Malus::getTileChar(){
    char malusChar[]{'|', 'm', 'W'};
    return malusChar[(int)mt];
}

//gives effect to the Maluses that the player has intersected with
void Malus::inflictMalusTo(Hero *hero){
    switch(mt){
        case MalusType::THORN:{
            hero->setHp(hero->getHp() - dp);
            quantity--;
            break;
        }
        case MalusType::LANDMINE:{
            //add *** animation
            hero->setHp(hero->getHp() - dp);
            quantity = 0;
            break;
        }
        case MalusType::BARBED_WIRE:{
            hero->setHp(hero->getHp() - dp);
            quantity--;
            break;
        }
        default:
        break;
    }
    if(quantity<=0) hp = 0;
}