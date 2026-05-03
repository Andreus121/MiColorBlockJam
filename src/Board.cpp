#include <iostream>
#include "Board.h"

Board::Board(StaticData* staticData,
        Block** blocks,
        Exit** exits,
        Gate** gates){
    //save dynamic data
    this->staticData = staticData;
    this->blocks = blocks;
    this->exits = exits;
    this->gates = gates;
    this->movesCount = 0;
    
    //save memory and build the grid
    rebuildGrid();
}

Board::~Board(){
    //free memory if the grid exist
    if(grid != nullptr){
        uint8_t height = staticData->boardHeight;
        for(uint8_t i = 0; i < height; i++){
            delete[] grid[i];
        }
        delete[] grid;
        grid = nullptr;
    }

    //free blocks
    for(uint8_t i = 0; i < blocksCount; i++){
        delete blocks[i];
    }
    delete[] blocks;

    //free exits
    for(uint8_t i = 0; i < staticData->exitCount; i++){
        delete exits[i];
    }
    delete[] exits;

    //free gates
    for(uint8_t i = 0; i < staticData->gateCount; i++){
        delete gates[i];
    }
    delete[] gates;

    //Don't free StaticData, the main do that
}

//rebuild the Grid with all of his characters
void Board::rebuildGrid(){
    int height = staticData->boardHeight;
    int width = staticData->boardWidth;

    //reserve memory for the grid
    if(grid == nullptr){
        grid = new char*[height];
        for(int i = 0; i < height; i++){
            grid[i] = new char[width];
        }
    }

    //copy the walls "#" in the grid
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            grid[i][j] = staticData->boardWalls[i][j];
        }
    }

    //pint exits with his orientatios V or H
    for(int s = 0; s < staticData->exitCount; s++){
        //hold data of the exit
        Exit* exit = exits[s];
        int x0 = staticData->exitX[s];
        int y0 = staticData->exitY[s];
        char color = staticData->exitColors[s];
        char orientation = staticData->exitOrientations[s];
        int actualLen = exit->actualLen;

        
        for(int k = 0; k < actualLen; k++){
            int fila, col;
            if(orientation == 'H'){
                fila = y0;
                col = x0 + k;
            } else {
                fila = y0 + k;
                col = x0;
            }
            //seguridad solo pintar dentro del tablero
            if(fila >= 0 && fila < height && col >= 0 && col < width){
                this->grid[fila][col] = color;
            }
        }
    }

    //pintar gates con su color actual
    for(int c = 0; c < this->staticData->gateCount; c++){
        int xc = this->staticData->xCompuertas[c];
        int yc = this->staticData->yCompuertas[c];
        this->grid[yc][xc] = this->gates[c]->coloractual;
    }

    //pintar blocks según su geometría
    //geometriaBloques[idBloque] es un arreglo plano de width*height booleanos
    //fila i, col j del bloque => index = i*anchoBloque + j
    for(int b = 0; b < this->blocksCount; b++){
        Block* bloque = this->blocks[b];
        int id = bloque->id;
        int anchoB = this->staticData->anchoBloques[id];
        int altoB = this->staticData->altoBloques[id];
        char colorB = this->staticData->coloresBloques[id];
        uint8_t* geom = this->staticData->geometriaBloques[id];

        for(int i = 0; i < altoB; i++){
            for(int j = 0; j < anchoB; j++){
                if(geom[i * anchoB + j] == 1){
                    int fila = bloque->y + i;
                    int col = bloque->x + j;
                    this->grid[fila][col] = colorB;
                }
            }
        }
    }
}

//muestra el tablero en pantalla
void Board::imprimir(){
    int height = this->staticData->boardHeight;
    int width = this->staticData->boardWidth;
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            std::cout << this->grid[i][j];
        }
        std::cout << std::endl;
    }
}

//los metodos usados para movimientos y A*

//comprobar si el juego termino
bool Board::esSolucion(){
    return this->blocksCount == 0;
}

//clonar el tablero actual
Board* Board::clonar(){
    //copiar blocks vivos
    Block** nuevosBloques = new Block*[this->blocksCount];
    for(int i = 0; i < this->blocksCount; i++){
        Block* orig = this->blocks[i];//crea el arreglo
        nuevosBloques[i] = new Block(orig->id, orig->x, orig->y, this->staticData);//agrega cada bloque
    }

    //copiar exits (todas, aunque no haya blocks adentro siguen existiendo)
    int numSalidas = this->staticData->exitCount;
    Exit** nuevasSalidas = new Exit*[numSalidas];
    for(int i = 0; i < numSalidas; i++){
        Exit* orig = this->exits[i];
        Exit* copia = new Exit(orig->id, orig->la, this->staticData);
        copia->contadorpasos = orig->contadorpasos;
        copia->direccion = orig->direccion;
        nuevasSalidas[i] = copia;
    }

    //copiar gates
    int numCompuertas = this->staticData->gateCount;
    Gate** nuevasCompuertas = new Gate*[numCompuertas];
    for(int i = 0; i < numCompuertas; i++){
        Gate* orig = this->gates[i];
        Gate* copia = new Gate(orig->id, orig->coloractual, this->staticData);
        copia->contadorpasos = orig->contadorpasos;
        nuevasCompuertas[i] = copia;
    }

    //crear nuevo tablero con this como padre
    Board* clon = new Board(
        this->staticData,
        this->blocksCount,
        nuevosBloques,
        nuevasSalidas,
        nuevasCompuertas,
        this->movesCount,
        this);
    return clon;
}

//retorna el indice del bloque con ese id o -1 si no existe
int Board::buscarBloque(uint8_t idBloque){
    for(int i = 0; i < this->blocksCount; i++){
        if((uint8_t)this->blocks[i]->id == idBloque){
            return i;
        }
    }
    return -1;
}

bool Board::comprobarMovimiento(uint8_t idBloque, char direccion){
    //buscar el indice del bloque
    int idx = buscarBloque(idBloque);
    //si no existe no se valida el movimiento
    if(idx == -1) return false;

    //obtener el puntero al bloque
    Block* bloque = this->blocks[idx];
    int id = bloque->id;
    int anchoB = this->staticData->anchoBloques[id];
    int altoB = this->staticData->altoBloques[id];
    uint8_t* geom = this->staticData->geometriaBloques[id];

    //desplazar 1 celda en la dirección dada
    int dx = 0, dy = 0;
    if(direccion == 'U') dy = -1;
    else if(direccion == 'D') dy = 1;
    else if(direccion == 'L') dx = -1;
    else if(direccion == 'R') dx = 1;
    else return false;

    //posición destino del bloque
    int nuevoX = bloque->x + dx;
    int nuevoY = bloque->y + dy;

    //recorrer cada celda ocupada por la geometría en la NUEVA posición
    for(int i = 0; i < altoB; i++){
        for(int j = 0; j < anchoB; j++){
            if(geom[i * anchoB + j] != 1){
                continue; // si la geometria no ocupa la celda, no chequeamos colisiones ahí
            }
            //calcular las coordenadas absolutas de la celda a validar
            int fila = nuevoY + i;
            int col = nuevoX + j;

            char contenido = this->grid[fila][col];
            if(contenido == ' '){
                continue; // vacío, ok
            }

            //la celda tiene algo, verificar si es parte del mismo bloque en su pos actual
            int localI = fila - bloque->y;//indice dentro de geometria
            int localJ = col - bloque->x;//indice dentro de geometria
            bool esMismoBloque = false;
            //comprobar limites de geometria
            if(localI >= 0 && localI < altoB && localJ >= 0 && localJ < anchoB){
                //verificar si es de la misma geometria
                if(geom[localI * anchoB + localJ] == 1){ 
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


bool Board::intentarSalida(uint8_t idBloque){
    //buscar el indice del bloque
    int idx = buscarBloque(idBloque);
    //si no existe no se puede salir
    if(idx == -1) return false;

    //guardar los datos del bloque
    Block* bloque = this->blocks[idx];
    int id = bloque->id;
    int anchoB = this->staticData->anchoBloques[id];
    int altoB = this->staticData->altoBloques[id];
    char colorB = this->staticData->coloresBloques[id];

    //ver todas las exits
    for(int s = 0; s < this->staticData->exitCount; s++){
        //color debe coincidir
        if(this->staticData->exitColors[s] != colorB) continue;

        //guardar datos de la salida
        int xs = staticData->exitX[s];
        int ys = staticData->exitY[s];
        char orientation = staticData->exitOrientations[s];
        int actualLen = exits[s]->la;

        //casos segun orientacion y lado del tablero
        //salida vertical (V) en la pared derecha o izquierda
        if(orientation == 'V'){
            //la salida ocupa filas desde ys hasta ys+actualLen-1, en columna xs
            //bloque a la derecha saliendo: borde derecho del bloque adyacente a xs
            //bloque a la izquierda saliendo: borde izquierdo del bloque adyacente a xs
            bool adyDerecha = (bloque->x + anchoB == xs);
            bool adyIzquierda = (bloque->x - 1 == xs);
            if(!adyDerecha && !adyIzquierda){
                continue;
            }
            //el height del bloque debe ser <= actualLen
            if(altoB > actualLen) {
                continue;
            }
            //las filas del bloque (y..y+altoB-1) deben estar DENTRO del rango de la salida
            if(bloque->y < ys){
                continue;
            }
            if(bloque->y + altoB > ys + actualLen){
                continue;
            }

            //todo ok: el bloque sale
            //eliminar el bloque del arreglo
            delete this->blocks[idx];
            for(int k = idx; k < this->blocksCount - 1; k++){
                this->blocks[k] = this->blocks[k + 1];
            }
            this->blocksCount--;
            return true;
        }
        //salida horizontal (H) en la pared arriba o abajo
        else {
            //la salida ocupa columnas xs..xs+actualLen-1, en fila ys
            bool adyAbajo = (bloque->y + altoB == ys);
            bool adyArriba = (bloque->y - 1 == ys);
            if(!adyAbajo && !adyArriba){
                continue;
            }

            //si el bloque no cabe, sigue
            if(anchoB > actualLen) {
                continue;
            }

            //comprobar que el bloque este entre xs y xs+actualLen-1
            if(bloque->x < xs) {
                continue;
            }
            if(bloque->x + anchoB > xs + actualLen) {
                continue;
            }

            //todo ok: el bloque sale
            //eliminar el bloque del arreglo
            delete this->blocks[idx];
            for(int k = idx; k < this->blocksCount - 1; k++){
                this->blocks[k] = this->blocks[k + 1];
            }
            this->blocksCount--;
            return true;
        }
    }
    return false;
}

bool Board::intentarCompuerta(uint8_t idBloque){
    //obtener el indice del bloque en el arreglo
    int idx = buscarBloque(idBloque);
    //si el bloque no existe, no puede cruzar compuerta
    if(idx == -1) return false;

    //guardar los datos del bloque
    Block* bloque = this->blocks[idx];
    int id = bloque->id;
    int anchoB = this->staticData->anchoBloques[id];
    int altoB = this->staticData->altoBloques[id];
    char colorB = this->staticData->coloresBloques[id];
    uint8_t* geom = this->staticData->geometriaBloques[id];

    //guardar datos del tablero
    int height = this->staticData->boardHeight;
    int width = this->staticData->boardWidth;

    //comprobar cada compuerta
    for(int c = 0; c < this->staticData->gateCount; c++){
        //color de la compuerta debe coincidir con el bloque
        if(this->gates[c]->coloractual != colorB){ 
            continue;
        }
        //guardar coordenadas de la compuerta
        int xc = this->staticData->xCompuertas[c];
        int yc = this->staticData->yCompuertas[c];

        //chequear si el bloque está adyacente a la compuerta
        //y calcular el desplazamiento del teletransporte
        int dx = 0, dy = 0;
        bool adyacente = false;

        //compuerta a la derecha del bloque
        if(bloque->x + anchoB == xc && bloque->y <= yc && yc < bloque->y + altoB){
            dx = anchoB + 1; adyacente = true;
        }
        //compuerta a la izquierda
        else if(bloque->x - 1 == xc && bloque->y <= yc && yc < bloque->y + altoB){
            dx = -(anchoB + 1); adyacente = true;
        }
        //compuerta abajo
        else if(bloque->y + altoB == yc && bloque->x <= xc && xc < bloque->x + anchoB){
            dy = altoB + 1; adyacente = true;
        }
        //compuerta arriba
        else if(bloque->y - 1 == yc && bloque->x <= xc && xc < bloque->x + anchoB){
            dy = -(altoB + 1); adyacente = true;
        }

        //si no está adyacente, seguir con la siguiente compuerta
        if(!adyacente){ 
            continue;
        }

        //chequear que en la nueva posición haya espacio para el bloque completo
        int nuevoX = bloque->x + dx;
        int nuevoY = bloque->y + dy;

        //verificar si hay espacio del otro lado de la compuerta
        bool hayEspacio = true;
        for(int i = 0; i < altoB && hayEspacio; i++){
            for(int j = 0; j < anchoB && hayEspacio; j++){
                //si la geometria del bloque no usa ese espacio, pasa de largo
                if(geom[i * anchoB + j] != 1){ 
                    continue;
                }
                //calcular la fila y columna de la nueva coordenada
                int fila = nuevoY + i;
                int col = nuevoX + j;
                //comprobar que la celda esté dentro del tablero
                if(fila < 0 || fila >= height || col < 0 || col >= width){
                    hayEspacio = false; break;
                }
                //comprobar que la celda esté vacía
                if(this->grid[fila][col] != ' '){
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

bool Board::moverBloque(uint8_t idBloque, char direccion){
    //verifica si se puede hacer el movimiento
    if(!comprobarMovimiento(idBloque, direccion)){
         return false;
    }
    //buscar el indice del bloque a mover
    int idx = buscarBloque(idBloque);
    //guardar el puntero del bloque a mover
    Block* bloque = this->blocks[idx];

    //aplicar el desplazamiento de 1 celda
    if(direccion == 'U') bloque->y -= 1;
    else if(direccion == 'D') bloque->y += 1;
    else if(direccion == 'L') bloque->x -= 1;
    else if(direccion == 'R') bloque->x += 1;

    
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
    this->movimientoOrigen = Movimiento(idBloque, direccion, 1);
    
    //reconstruir grid con la nueva posición + tiempo avanzado
    rebuildGrid();

    //ahora chequear eventos post-movimiento (sin costo adicional)
    if(intentarSalida(idBloque)){
        rebuildGrid(); //se eliminó el bloque
        return true;
    }
    if(intentarCompuerta(idBloque)){
        rebuildGrid(); //cambió la posición
        return true;
    }

    return true;
}

//Libera el arreglo de grid para ahorrar memoria.
//Usado por A* al mover tableros al ClosedSet: alli solo nos interesa
//comparar estados por hash/esIgual y seguir punteros padre, no ver la visual.
//Si alguien despues llama imprimir() o moverBloque() sobre este tablero,
//rebuildGrid() la regenerara automaticamente.
void Board::liberarCuadricula(){
    if(this->grid == nullptr) return;
    int height = this->staticData->boardHeight;
    for(int i = 0; i < height; i++){
        delete[] this->grid[i];
    }
    delete[] this->grid;
    this->grid = nullptr;
}