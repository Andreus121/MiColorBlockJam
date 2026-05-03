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
        int8_t xc = staticData->gateX[c];
        int8_t yc = staticData->gateY[c];
        grid[yc][xc] = gates[c]->actualColor;
    }

    //pint blocks with his geometric, geometric is an array []
    for(int8_t b = 0; b < blocksCount; b++){
        Block* bloque = blocks[b];
        int8_t id = bloque->id;
        int8_t blockWidth = staticData->blockWidth[id];
        int8_t blockHeight = staticData->blockHeight[id];
        char blockColor = staticData->blockColors[id];
        int8_t* blockGeometric = staticData->blockGeometrics[id];

        for(int8_t i = 0; i < blockHeight; i++){
            for(int8_t j = 0; j < blockWidth; j++){
                if(blockGeometric[i * blockWidth + j] == 1){
                    int8_t row = bloque->y + i;
                    int8_t column = bloque->x + j;
                    grid[row][column] = blockColor;
                }
            }
        }
    }
}

void Board::printGrid(){
    int height = this->staticData->boardHeight;
    int width = this->staticData->boardWidth;
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
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
    for(int i = 0; i < blocksCount; i++){
        Block* original = blocks[i];//crea el arreglo
        newBlocks[i] = new Block(original->id, original->x, original->y, staticData);
    }

    //copy exits
    int exitCount = staticData->exitCount;
    Exit** newExits = new Exit*[exitCount];
    for(int i = 0; i < exitCount; i++){
        Exit* original = exits[i];
        Exit* copy = new Exit(original->id, original->actualLen, this->staticData);
        copy->stepsCount = original->stepsCount;
        copy->direction = original->direction;
        newExits[i] = copy;
    }

    //copy gates
    int gateCount = staticData->gateCount;
    Gate** nuevasCompuertas = new Gate*[gateCount];
    for(int i = 0; i < gateCount; i++){
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
int8_t Board::findBlock(int8_t idBloque){
    for(int8_t i = 0; i < blocksCount; i++){
        if((int8_t)blocks[i]->id == idBloque){
            return i;
        }
    }
    return -1;
}

bool Board::canMove(int8_t blockID, char direction){
    //find the index of the block
    int index = findBlock(blockID);
    //if dont exist, dont can move
    if(index == -1) return false;

    //obtener el puntero al bloque
    Block* bloque = this->blocks[index];
    int id = bloque->id;
    int blockWidth = this->staticData->blockWidth[id];
    int blockHeight = this->staticData->blockHeight[id];
    int8_t* blockGeometric = this->staticData->blockGeometrics[id];

    //desplazar 1 celda en actualLen dirección dada
    int dx = 0, dy = 0;
    if(direction == 'U') dy = -1;
    else if(direction == 'D') dy = 1;
    else if(direction == 'L') dx = -1;
    else if(direction == 'R') dx = 1;
    else return false;

    //posición destino del bloque
    int nuevoX = bloque->x + dx;
    int nuevoY = bloque->y + dy;

    //recorrer cada celda ocupada por actualLen geometría en actualLen NUEVA posición
    for(int i = 0; i < blockHeight; i++){
        for(int j = 0; j < blockWidth; j++){
            if(blockGeometric[i * blockWidth + j] != 1){
                continue; // si actualLen geometria no ocupa actualLen celda, no chequeamos colisiones ahí
            }
            //calcular las coordenadas absolutas de actualLen celda a validar
            int row = nuevoY + i;
            int column = nuevoX + j;

            char contenido = this->grid[row][column];
            if(contenido == ' '){
                continue; // vacío, ok
            }

            //actualLen celda tiene algo, verificar si es parte del mismo bloque en su pos actual
            int localI = row - bloque->y;//indice dentro de geometria
            int localJ = column - bloque->x;//indice dentro de geometria
            bool esMismoBloque = false;
            //comprobar limites de geometria
            if(localI >= 0 && localI < blockHeight && localJ >= 0 && localJ < blockWidth){
                //verificar si es de actualLen misma geometria
                if(blockGeometric[localI * blockWidth + localJ] == 1){ 
                    esMismoBloque = true;
                }
            }
            if(!esMismoBloque) {
                return false;
            } //choca con algo ajeno
        }
    }
    return true; //no choca con nada
}


bool Board::intentarSalida(int8_t idBloque){
    //buscar el indice del bloque
    int index = buscarBloque(idBloque);
    //si no existe no se puede salir
    if(index == -1) return false;

    //guardar los datos del bloque
    Block* bloque = this->blocks[index];
    int id = bloque->id;
    int blockWidth = this->staticData->blockWidth[id];
    int blockHeight = this->staticData->blockHeight[id];
    char blockColor = this->staticData->blockColors[id];

    //ver todas las exits
    for(int s = 0; s < this->staticData->exitCount; s++){
        //color debe coincidir
        if(this->staticData->exitColors[s] != blockColor) continue;

        //guardar datos de actualLen salida
        int xs = staticData->exitX[s];
        int ys = staticData->exitY[s];
        char orientation = staticData->exitOrientations[s];
        int actualLen = exits[s]->actualLen;

        //casos segun orientacion y lado del tablero
        //salida vertical (V) en actualLen pared derecha o izquierda
        if(orientation == 'V'){
            //actualLen salida ocupa filas desde ys hasta ys+actualLen-1, en columna xs
            //bloque a actualLen derecha saliendo: borde derecho del bloque adyacente a xs
            //bloque a actualLen izquierda saliendo: borde izquierdo del bloque adyacente a xs
            bool adyDerecha = (bloque->x + blockWidth == xs);
            bool adyIzquierda = (bloque->x - 1 == xs);
            if(!adyDerecha && !adyIzquierda){
                continue;
            }
            //el height del bloque debe ser <= actualLen
            if(blockHeight > actualLen) {
                continue;
            }
            //las filas del bloque (y..y+blockHeight-1) deben estar DENTRO del rango de actualLen salida
            if(bloque->y < ys){
                continue;
            }
            if(bloque->y + blockHeight > ys + actualLen){
                continue;
            }

            //todo ok: el bloque sale
            //eliminar el bloque del arreglo
            delete this->blocks[index];
            for(int k = index; k < this->blocksCount - 1; k++){
                this->blocks[k] = this->blocks[k + 1];
            }
            this->blocksCount--;
            return true;
        }
        //salida horizontal (H) en actualLen pared arriba o abajo
        else {
            //actualLen salida ocupa columnas xs..xs+actualLen-1, en row ys
            bool adyAbajo = (bloque->y + blockHeight == ys);
            bool adyArriba = (bloque->y - 1 == ys);
            if(!adyAbajo && !adyArriba){
                continue;
            }

            //si el bloque no cabe, sigue
            if(blockWidth > actualLen) {
                continue;
            }

            //comprobar que el bloque este entre xs y xs+actualLen-1
            if(bloque->x < xs) {
                continue;
            }
            if(bloque->x + blockWidth > xs + actualLen) {
                continue;
            }

            //todo ok: el bloque sale
            //eliminar el bloque del arreglo
            delete this->blocks[index];
            for(int k = index; k < this->blocksCount - 1; k++){
                this->blocks[k] = this->blocks[k + 1];
            }
            this->blocksCount--;
            return true;
        }
    }
    return false;
}

bool Board::intentarCompuerta(int8_t idBloque){
    //obtener el indice del bloque en el arreglo
    int index = buscarBloque(idBloque);
    //si el bloque no existe, no puede cruzar compuerta
    if(index == -1) return false;

    //guardar los datos del bloque
    Block* bloque = this->blocks[index];
    int id = bloque->id;
    int blockWidth = this->staticData->blockWidth[id];
    int blockHeight = this->staticData->blockHeight[id];
    char blockColor = this->staticData->blockColors[id];
    int8_t* blockGeometric = this->staticData->blockGeometrics[id];

    //guardar datos del tablero
    int height = this->staticData->boardHeight;
    int width = this->staticData->boardWidth;

    //comprobar cada compuerta
    for(int c = 0; c < this->staticData->gateCount; c++){
        //color de actualLen compuerta debe coincidir con el bloque
        if(this->gates[c]->actualColor != blockColor){ 
            continue;
        }
        //guardar coordenadas de actualLen compuerta
        int xc = this->staticData->gateX[c];
        int yc = this->staticData->gateY[c];

        //chequear si el bloque está adyacente a actualLen compuerta
        //y calcular el desplazamiento del teletransporte
        int dx = 0, dy = 0;
        bool adyacente = false;

        //compuerta a actualLen derecha del bloque
        if(bloque->x + blockWidth == xc && bloque->y <= yc && yc < bloque->y + blockHeight){
            dx = blockWidth + 1; adyacente = true;
        }
        //compuerta a actualLen izquierda
        else if(bloque->x - 1 == xc && bloque->y <= yc && yc < bloque->y + blockHeight){
            dx = -(blockWidth + 1); adyacente = true;
        }
        //compuerta abajo
        else if(bloque->y + blockHeight == yc && bloque->x <= xc && xc < bloque->x + blockWidth){
            dy = blockHeight + 1; adyacente = true;
        }
        //compuerta arriba
        else if(bloque->y - 1 == yc && bloque->x <= xc && xc < bloque->x + blockWidth){
            dy = -(blockHeight + 1); adyacente = true;
        }

        //si no está adyacente, seguir con actualLen siguiente compuerta
        if(!adyacente){ 
            continue;
        }

        //chequear que en actualLen nueva posición haya espacio para el bloque completo
        int nuevoX = bloque->x + dx;
        int nuevoY = bloque->y + dy;

        //verificar si hay espacio del otro lado de actualLen compuerta
        bool hayEspacio = true;
        for(int i = 0; i < blockHeight && hayEspacio; i++){
            for(int j = 0; j < blockWidth && hayEspacio; j++){
                //si actualLen geometria del bloque no usa ese espacio, pasa de largo
                if(blockGeometric[i * blockWidth + j] != 1){ 
                    continue;
                }
                //calcular actualLen row y columna de actualLen nueva coordenada
                int row = nuevoY + i;
                int column = nuevoX + j;
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
        bloque->x = nuevoX;
        bloque->y = nuevoY;
        return true;//si pudo pasar el bloque
    }
    return false;//no pudo pasar el bloque
}

bool Board::moverBloque(int8_t idBloque, char direction){
    //verifica si se puede hacer el movimiento
    if(!comprobarMovimiento(idBloque, direction)){
         return false;
    }
    //buscar el indice del bloque a mover
    int index = buscarBloque(idBloque);
    //guardar el puntero del bloque a mover
    Block* bloque = this->blocks[index];

    //aplicar el desplazamiento de 1 celda
    if(direction == 'U') bloque->y -= 1;
    else if(direction == 'D') bloque->y += 1;
    else if(direction == 'L') bloque->x -= 1;
    else if(direction == 'R') bloque->x += 1;

    
    //actualizar gates y exits según su contador de pasos
    for(int c = 0; c < this->staticData->gateCount; c++){
        this->gates[c]->actualizarCompuerta(
            this->staticData->colorICompuertas[c],
            this->staticData->colorFCompuertas[c],
            this->staticData->pasosCompuertas[c]);
    }
    for(int s = 0; s < this->staticData->exitCount; s++){
        this->exits[s]->actualizarSalida(
            this->staticData->largoISalidas[s],
            this->staticData->largoFSalidas[s],
            this->staticData->pasosSalidas[s]);
    }
    //avanzar el tiempo: 1 paso por celda deslizada
    this->movesCount += 1;

    //registrar el movimiento
    this->movimientoOrigen = Movimiento(idBloque, direction, 1);
    
    //reconstruir grid con actualLen nueva posición + tiempo avanzado
    rebuildGrid();

    //ahora chequear eventos post-movimiento (sin costo adicional)
    if(intentarSalida(idBloque)){
        rebuildGrid(); //se eliminó el bloque
        return true;
    }
    if(intentarCompuerta(idBloque)){
        rebuildGrid(); //cambió actualLen posición
        return true;
    }

    return true;
}

//Libera el arreglo de grid para ahorrar memoria.
//Usado por A* al mover tableros al ClosedSet: alli solo nos interesa
//comparar estados por hash/esIgual y seguir punteros padre, no ver actualLen visual.
//Si alguien despues llama imprimir() o moverBloque() sobre este tablero,
//rebuildGrid() actualLen regenerara automaticamente.
void Board::liberarCuadricula(){
    if(this->grid == nullptr) return;
    int height = this->staticData->boardHeight;
    for(int i = 0; i < height; i++){
        delete[] this->grid[i];
    }
    delete[] this->grid;
    this->grid = nullptr;
}