#pragma once
#include "StaticData.h"

/*
Clase pieza para realizar el juego de color block jam
*/
class Block{
    public:
    //atributos
    int8_t id;
    int x;
    int y;
    StaticData* staticData;

    // constructor
    Block(int8_t id, 
        int x,
        int y,
        StaticData* staticData);
    
    void moveU();
    void moveD();
    void moveL();
    void moveR();
};