#pragma once
#include <cstdint>
#include "StaticData.h"

/*
Clase pieza para realizar el juego de color block jam
*/
class Bloque {
    public:
    //atributos
    uint8_t id;
    int x;
    int y;
    StaticData* staticData;

    // constructor
    Bloque(uint8_t id, 
        int x,
        int y,
        StaticData* staticData);
    
    void moverArriba();
    void moverAbajo();
    void moverIzquierda();
    void moverDerecha();
};