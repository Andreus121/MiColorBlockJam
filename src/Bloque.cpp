#include "Bloque.h"

Bloque::Bloque(uint8_t id, 
        int x,
        int y,
        StaticData* staticData){
    this->id = id;
    this->x = x;
    this->y = y;
    this->staticData = staticData;
};

void Bloque::moverArriba(){
    this->y -= 1;
}

void Bloque::moverAbajo(){
    this->y += 1;
}

void Bloque::moverIzquierda(){
    this->x -= 1;
}

void Bloque::moverDerecha(){
    this->x += 1;
}
