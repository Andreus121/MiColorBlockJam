#include <iostream>
#include "Block.h"

int main(){
    Block* block = new Block(1,0,0,nullptr);

    std::cout << "id: "<< block->id<<" coords: "<<
    block->x << block->y << std::endl;

    std::cout<< "Move the block"<<std::endl;
    block->moveU();
    std::cout<< "UP "<< block->x << block->y << std::endl;
    block->moveD();
    std::cout<< "DOWN "<< block->x << block->y << std::endl;
    block->moveL();
    std::cout<< "LEFT "<< block->x << block->y << std::endl;
    block->moveR();
    std::cout<< "RIGHT "<< block->x << block->y << std::endl;

    delete block;
    return 0;
}