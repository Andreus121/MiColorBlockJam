#pragma once
#include "StaticData.h"
#include <cstdint>

//Holds the dynamic data of a block. (position, id)
//Static data lives in StaticData
class Block{
    public:
    uint8_t id;
    int8_t x;
    int8_t y;
    StaticData* staticData;

    //constructor
    Block(uint8_t id, 
        int8_t x,
        int8_t y,
        StaticData* staticData);
    
    //move
    void moveU();
    void moveD();
    void moveL();
    void moveR();
};