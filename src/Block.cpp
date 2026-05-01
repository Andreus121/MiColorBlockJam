#include "Block.h"
//Holds the dynamic data of a block. (position, id)
//Static data lives in StaticData
Block::Block(int8_t id, 
        int x,
        int y,
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