#include <iostream>
#include "Board.h"

Board::Board(StaticData* staticData, Block** blocks,
        Exit** exits, Gate** gates, 
        uint8_t movesCount, int8_t blocksCount){
    //save dynamic data
    this->staticData = staticData;
    this->blocksCount = blocksCount;//default = 0
    this->blocks = blocks;
    this->exits = exits;
    this->gates = gates;
    this->movesCount = movesCount;//default = 0
    
    //save memory and build the grid
    rebuildGrid();
}

Board::~Board(){
    //free memory if the grid exist
    if(grid != nullptr){
        int8_t height = staticData->boardHeight;
        for(int8_t i = 0; i < height; i++){
            delete[] grid[i];
        }
        delete[] grid;
        grid = nullptr;
    }

    //free blocks
    for(int8_t i = 0; i < blocksCount; i++){
        delete blocks[i];
    }
    delete[] blocks;

    //free exits
    for(int8_t i = 0; i < staticData->exitCount; i++){
        delete exits[i];
    }
    delete[] exits;

    //free gates
    for(int8_t i = 0; i < staticData->gateCount; i++){
        delete gates[i];
    }
    delete[] gates;

    //Don't free StaticData, the main do that
}

//rebuild the Grid with all of his characters
void Board::rebuildGrid(){
    int8_t height = staticData->boardHeight;
    int8_t width = staticData->boardWidth;

    //reserve memory for the grid
    if(grid == nullptr){
        grid = new char*[height];
        for(int8_t i = 0; i < height; i++){
            grid[i] = new char[width];
        }
    }

    //copy the walls "#" in the grid
    for(int8_t i = 0; i < height; i++){
        for(int8_t j = 0; j < width; j++){
            grid[i][j] = staticData->boardWalls[i][j];
        }
    }

    //pint exits with his orientatios V or H
    for(int8_t s = 0; s < staticData->exitCount; s++){
        //hold data of the exit
        Exit* exit = exits[s];
        int8_t x0 = staticData->exitX[s];
        int8_t y0 = staticData->exitY[s];
        char color = staticData->exitColors[s];
        char orientation = staticData->exitOrientations[s];
        int8_t actualLen = exit->actualLen;

        for(int8_t k = 0; k < actualLen; k++){
            int8_t row, column;
            //pint Horizontal exit
            if(orientation == 'H'){
                row = y0;
                column = x0 + k;
            }
            //pint Vertical exit
            else{
                row = y0 + k;
                column = x0;
            }
            //only pint in the board
            if(row >= 0 && row < height && column >= 0 && column < width){
                grid[row][column] = color;
            }
        }
    }

    //pint gates
    for(int8_t c = 0; c < staticData->gateCount; c++){
        int8_t gateX = staticData->gateX[c];
        int8_t gateY = staticData->gateY[c];
        grid[gateY][gateX] = gates[c]->actualColor;
    }

    //pint blocks with his geometric, geometric is an array []
    for(int8_t b = 0; b < blocksCount; b++){
        Block* block = blocks[b];
        int8_t id = block->id;
        int8_t blockWidth = staticData->blockWidth[id];
        int8_t blockHeight = staticData->blockHeight[id];
        char blockColor = staticData->blockColors[id];
        uint8_t* blockGeometric = staticData->blockGeometrics[id];

        for(int8_t i = 0; i < blockHeight; i++){
            for(int8_t j = 0; j < blockWidth; j++){
                if(blockGeometric[i * blockWidth + j] == 1){
                    int8_t row = block->y + i;
                    int8_t column = block->x + j;
                    grid[row][column] = blockColor;
                }
            }
        }
    }
}

void Board::printGrid(){
    int8_t height = this->staticData->boardHeight;
    int8_t width = this->staticData->boardWidth;
    for(int8_t i = 0; i < height; i++){
        for(int8_t j = 0; j < width; j++){
            std::cout << this->grid[i][j];
        }
        std::cout << std::endl;
    }
}

bool Board::isSolved(){
    return blocksCount == 0;
}

//clone all data of the Table except the grid
Board* Board::clone(){
    //copy blocks
    Block** newBlocks = new Block*[blocksCount];
    for(int8_t i = 0; i < blocksCount; i++){
        Block* original = blocks[i];//crea el arreglo
        newBlocks[i] = new Block(original->id, original->x, original->y, staticData);
    }

    //copy exits
    int8_t exitCount = staticData->exitCount;
    Exit** newExits = new Exit*[exitCount];
    for(int8_t i = 0; i < exitCount; i++){
        Exit* original = exits[i];
        Exit* copy = new Exit(original->id, original->actualLen, this->staticData);
        copy->stepsCount = original->stepsCount;
        copy->direction = original->direction;
        newExits[i] = copy;
    }

    //copy gates
    int8_t gateCount = staticData->gateCount;
    Gate** nuevasCompuertas = new Gate*[gateCount];
    for(int8_t i = 0; i < gateCount; i++){
        Gate* original = this->gates[i];
        Gate* copy = new Gate(original->id, original->actualColor, staticData);
        copy->stepsCount = original->stepsCount;
        nuevasCompuertas[i] = copy;
    }

    //create the new board
    Board* clon = new Board(staticData,
        newBlocks,
        newExits,
        nuevasCompuertas,
        movesCount,
        blocksCount);
    return clon;
}

//return the id of the block or -1 if the block dont exist in the board
int8_t Board::findBlock(int8_t blockID){
    for(int8_t i = 0; i < blocksCount; i++){
        if((int8_t)blocks[i]->id == blockID){
            return i;
        }
    }
    return -1;
}

bool Board::canMove(int8_t blockID, char direction){
    //find the index of the block
    int8_t index = findBlock(blockID);
    //if dont exist, dont can move
    if(index == -1) return false;

    //get the data of the block
    Block* block = this->blocks[index];
    int8_t id = block->id;
    int8_t blockWidth = this->staticData->blockWidth[id];
    int8_t blockHeight = this->staticData->blockHeight[id];
    uint8_t* blockGeometric = staticData->blockGeometrics[id];

    //prepare the move of the block
    int8_t dx = 0, dy = 0;
    if(direction == 'U') dy = -1;
    else if(direction == 'D') dy = 1;
    else if(direction == 'L') dx = -1;
    else if(direction == 'R') dx = 1;
    else return false;

    //new position of the block
    int8_t newX = block->x + dx;
    int8_t newY = block->y + dy;

    //verify the new coords of the blockGeometrics in the grid
    for(int8_t i = 0; i < blockHeight; i++){
        for(int8_t j = 0; j < blockWidth; j++){
            //if the blockGeometrics don't use that space, continue
            if(blockGeometric[i * blockWidth + j] != 1) continue; 

            //prepare the exactly new coords
            int8_t row = newY + i;
            int8_t column = newX + j;
            //the space is not fill, continue
            if(grid[row][column] == ' ') continue;

            //the space is fill, verify if is the same block
            //index in the block
            int8_t localI = row - block->y;
            int8_t localJ = column - block->x;
            bool isTheSameBlock = false;
            //verify geometry limits
            if(localI >= 0 && localI < blockHeight && localJ >= 0 && localJ < blockWidth){
                //verify if the space used is the samen block
                if(blockGeometric[localI * blockWidth + localJ] == 1){ 
                    isTheSameBlock = true;
                }
            }
            //if is not the same block, the block can't move
            if(!isTheSameBlock) return false;
        }
    }
    return true; //the block can move
}

bool Board::tryExit(int8_t blockID){
    //find the index of the block
    int8_t index = findBlock(blockID);
    //if the index dont exist, dont try the exit
    if(index == -1) return false;

    //hold the data block
    Block* block = this->blocks[index];
    int8_t blockWidth = staticData->blockWidth[blockID];
    int8_t blockHeight = staticData->blockHeight[blockID];
    char blockColor = staticData->blockColors[blockID];

    //try every exit
    for(int8_t s = 0; s < this->staticData->exitCount; s++){
        //exit color and the block color are diferents, continue
        if(this->staticData->exitColors[s] != blockColor) continue;

        //hold the data exit
        int8_t xs = staticData->exitX[s];
        int8_t ys = staticData->exitY[s];
        char orientation = staticData->exitOrientations[s];
        int8_t actualLen = exits[s]->actualLen;

        //vertical exit (V)
        if(orientation == 'V'){
            //the exit its in the right or  left the block?
            bool right = (block->x + blockWidth == xs);
            bool left= (block->x - 1 == xs);

            //the block dont touch the exit
            if(!right && !left) continue;

            //if the blocks have more height than the exit
            if(blockHeight > actualLen) continue;

            //the the block is further to up
            if(block->y < ys) continue;

            //the the block is further to down
            if(block->y + blockHeight > ys + actualLen) continue;

            //all ok, remove the block
            delete this->blocks[index];
            for(int8_t k = index; k < this->blocksCount - 1; k++){
                this->blocks[k] = this->blocks[k + 1];
            }
            this->blocksCount--;

            //the block has been removed
            return true;
        }
        //horizontal exit (H)
        else {
            //the exit its on or under the block?
            bool down = (block->y + blockHeight == ys);
            bool up = (block->y - 1 == ys);

            //the block dont touch the exit
            if(!down && !up) continue;

            //the block have more width than the exit
            if(blockWidth > actualLen) continue;

            //the block is further to the right
            if(block->x < xs) continue;

            //the the block is further to the left
            if(block->x + blockWidth > xs + actualLen) continue;

            //all ok, remove the block
            delete blocks[index];
            for(int8_t k = index; k < blocksCount - 1; k++){
                blocks[k] = blocks[k + 1];
            }
            blocksCount--;
            
            //the block has been removed
            return true;
        }
    }
    //the block was'nt been removed
    return false;
}

bool Board::tryGate(int8_t blockID){
    //find the index of the block
    int8_t index = findBlock(blockID);

    //if the block dont exist
    if(index == -1) return false;

    //hold block's data
    Block* block = blocks[index];
    int8_t blockWidth = staticData->blockWidth[blockID];
    int8_t blockHeight = staticData->blockHeight[blockID];
    char blockColor = staticData->blockColors[blockID];
    uint8_t* blockGeometric = staticData->blockGeometrics[blockID];

    //hold board's data
    int8_t height = staticData->boardHeight;
    int8_t width = staticData->boardWidth;

    //try every gate
    for(int8_t c = 0; c < staticData->gateCount; c++){
        //if the color is not the same, continue
        if(gates[c]->actualColor != blockColor) continue;

        //hold gate's coords
        int8_t gateX = staticData->gateX[c];
        int8_t gateY = staticData->gateY[c];

        //the block is near of the gate
        int8_t dx = 0, dy = 0;
        bool near = false;

        //the gate is in the right of the block
        if(block->x + blockWidth == gateX && block->y <= gateY && gateY < block->y + blockHeight){
            dx = blockWidth + 1; near = true;
        }
        //the gate is in the left of the block
        else if(block->x - 1 == gateX && block->y <= gateY && gateY < block->y + blockHeight){
            dx = -(blockWidth + 1); near = true;
        }
        //the gate is under the block
        else if(block->y + blockHeight == gateY && block->x <= gateX && gateX < block->x + blockWidth){
            dy = blockHeight + 1; near = true;
        }
        //the gate is on the block
        else if(block->y - 1 == gateY && block->x <= gateX && gateX < block->x + blockWidth){
            dy = -(blockHeight + 1); near = true;
        }

        //if the block is'nt near, continue
        if(!near) continue;

        //holds the new coord of the block
        int8_t newX = block->x + dx;
        int8_t newY = block->y + dy;

        //verify the space in the new coords
        bool hayEspacio = true;
        for(int8_t i = 0; i < blockHeight && hayEspacio; i++){
            for(int8_t j = 0; j < blockWidth && hayEspacio; j++){
                //si actualLen geometria del block no usa ese espacio, pasa de largo
                if(blockGeometric[i * blockWidth + j] != 1){ 
                    continue;
                }
                //calcular actualLen row y columna de actualLen nueva coordenada
                int8_t row = newY + i;
                int8_t column = newX + j;
                //comprobar que actualLen celda esté dentro del tablero
                if(row < 0 || row >= height || column < 0 || column >= width){
                    hayEspacio = false; break;
                }
                //comprobar que actualLen celda esté vacía
                if(this->grid[row][column] != ' '){
                    hayEspacio = false;
                }
            }
        }

        //si hay espacio, pasa de largo
        if(!hayEspacio){
        continue;
        }

        //aplicar teletransporte
        block->x = newX;
        block->y = newY;
        return true;//si pudo pasar el block
    }
    return false;//no pudo pasar el block
}

bool Board::moveBlock(int8_t blockID, char direction){
    //verify if the block can move
    if(!canMove(blockID, direction)) return false;

    //find the index of the block
    int8_t index = findBlock(blockID);

    //hold the block
    Block* block = this->blocks[index];

    //do the move
    if(direction == 'U') block->y -= 1;
    else if(direction == 'D') block->y += 1;
    else if(direction == 'L') block->x -= 1;
    else if(direction == 'R') block->x += 1;

    //update exits
    for(int8_t s = 0; s < this->staticData->exitCount; s++){
        this->exits[s]->updateExit();
    }

    //update gates
    for(int8_t c = 0; c < this->staticData->gateCount; c++){
            gates[c]->updateGate();
    }

    //update moves count
    this->movesCount += 1;

    //update grid
    rebuildGrid();

    //check if the block take a exit
    if(tryExit(blockID)){
        rebuildGrid(); //update grid
    }

    //check if the block take a gate
    if(tryGate(blockID)){
        rebuildGrid(); //update grid
    }

    return true;
}

//free memory of the grid
void Board::freeGrid(){
    //the grid don't exist
    if(this->grid == nullptr) return;
    //remove all characters of the grid
    int8_t height = this->staticData->boardHeight;
    for(int8_t i = 0; i < height; i++){
        delete[] this->grid[i];
    }
    delete[] this->grid;
    this->grid = nullptr;
}