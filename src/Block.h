#pragma once
#include "StaticData.h"

/*
Clase pieza para realizar el juego de color block jam
*/
class Bloque {
    public:
    //atributos
    int8_t id;
    int x;
    int y;
    StaticData* staticData;

    // constructor
    Bloque(int8_t id, 
        int x,
        int y,
        StaticData* staticData);
    
    void moverArriba();
    void moverAbajo();
    void moverIzquierda();
    void moverDerecha();
};