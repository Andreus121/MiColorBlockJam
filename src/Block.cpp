#include "Block.h"

Block::Block(uint8_t id, 
        int8_t x,
        int8_t y,
        StaticData* staticData){
    this->id = id;
    this->x = x;
    this->y = y;
    this->staticData = staticData;
};

void Block::moveU(){
    this->y -= 1;
}

void Block::moveD(){
    this->y += 1;
}

void Block::moveL(){
    this->x -= 1;
}

void Block::moveR(){
    this->x += 1;
}