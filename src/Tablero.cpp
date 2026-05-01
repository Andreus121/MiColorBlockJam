#include <iostream>
#include "Tablero.h"

Tablero::Tablero(StaticData* staticData,
        Bloque** bloques,
        Salida** salidas,
        Compuerta** compuertas){
    this->staticData = staticData;
    this->bloques = bloques;
    this->salidas = salidas;
    this->compuertas = compuertas;

    //el estado inicial parte con todos los bloques en juego
    this->cantidadBloques = staticData->cantidadBloques;
    this->contadorMovimientos = 0;

    //estado inicial no tiene padre ni movimiento previo
    this->padre = nullptr;
    this->movimientoOrigen = Movimiento();

    //reservar cuadricula vacía; se llena con reconstruirCuadricula()
    int alto = staticData->altoTablero;
    int ancho = staticData->anchoTablero;
    this->cuadricula = new char*[alto];
    for(int i = 0; i < alto; i++){
        this->cuadricula[i] = new char[ancho];
    }

    reconstruirCuadricula();
}

Tablero::~Tablero(){
    //liberar cuadricula
    //(puede haber sido liberada por liberarCuadricula() para ahorrar memoria durante A*
    if(this->cuadricula != nullptr){
        int alto = staticData->altoTablero;
        for(int i = 0; i < alto; i++){
            delete[] this->cuadricula[i];
        }
        delete[] this->cuadricula;
        this->cuadricula = nullptr;
    }

    //liberar bloques vivos
    for(int i = 0; i < this->cantidadBloques; i++){
        delete this->bloques[i];
    }
    delete[] this->bloques;

    //liberar salidas
    for(int i = 0; i < this->staticData->cantidadSalidas; i++){
        delete this->salidas[i];
    }
    delete[] this->salidas;

    //liberar compuertas
    for(int i = 0; i < this->staticData->cantidadCompuertas; i++){
        delete this->compuertas[i];
    }
    delete[] this->compuertas;

    //staticData NO se libera aquí: lo comparten todos los tableros
    //padre NO se libera aquí: la gestión de nodos la hace A*
}

//rellena todo el tablero
void Tablero::reconstruirCuadricula(){
    int alto = this->staticData->altoTablero;
    int ancho = this->staticData->anchoTablero;

    //si la cuadricula fue liberada (por ahorro de memoria), la reservamos de nuevo
    if(this->cuadricula == nullptr){
        this->cuadricula = new char*[alto];
        for(int i = 0; i < alto; i++){
            this->cuadricula[i] = new char[ancho];
        }
    }

    //copiar paredes base desde staticData
    //'#' pared, ' ' vacío
    for(int i = 0; i < alto; i++){
        for(int j = 0; j < ancho; j++){
            this->cuadricula[i][j] = this->staticData->paredesTablero[i][j];
        }
    }

    //pintar salidas según su largo actual
    //orientación 'H' (horizontal): ocupa celdas en x, y es fijo
    //orientación 'V' (vertical): ocupa celdas en y, x es fijo
    for(int s = 0; s < this->staticData->cantidadSalidas; s++){
        Salida* salida = this->salidas[s];
        int x0 = this->staticData->xSalidas[s];
        int y0 = this->staticData->ySalidas[s];
        char color = this->staticData->coloresSalidas[s];
        char orient = this->staticData->orientacionSalidas[s];
        int largoActual = salida->la;

        for(int k = 0; k < largoActual; k++){
            int fila, col;
            if(orient == 'H'){
                fila = y0;
                col = x0 + k;
            } else {
                fila = y0 + k;
                col = x0;
            }
            //seguridad solo pintar dentro del tablero
            if(fila >= 0 && fila < alto && col >= 0 && col < ancho){
                this->cuadricula[fila][col] = color;
            }
        }
    }

    //pintar compuertas con su color actual
    for(int c = 0; c < this->staticData->cantidadCompuertas; c++){
        int xc = this->staticData->xCompuertas[c];
        int yc = this->staticData->yCompuertas[c];
        this->cuadricula[yc][xc] = this->compuertas[c]->coloractual;
    }

    //pintar bloques según su geometría
    //geometriaBloques[idBloque] es un arreglo plano de ancho*alto booleanos
    //fila i, col j del bloque => index = i*anchoBloque + j
    for(int b = 0; b < this->cantidadBloques; b++){
        Bloque* bloque = this->bloques[b];
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
                    this->cuadricula[fila][col] = colorB;
                }
            }
        }
    }
}

//muestra el tablero en pantalla
void Tablero::imprimir(){
    int alto = this->staticData->altoTablero;
    int ancho = this->staticData->anchoTablero;
    for(int i = 0; i < alto; i++){
        for(int j = 0; j < ancho; j++){
            std::cout << this->cuadricula[i][j];
        }
        std::cout << std::endl;
    }
}

//los metodos usados para movimientos y A*

//comprobar si el juego termino
bool Tablero::esSolucion(){
    return this->cantidadBloques == 0;
}

//constructor usado por A* para clonar
Tablero::Tablero(StaticData* staticData,
        int cantidadBloques,
        Bloque** bloques,
        Salida** salidas,
        Compuerta** compuertas,
        int contadorMovimientos,
        Tablero* padre){
    this->staticData = staticData;
    this->cantidadBloques = cantidadBloques;
    this->bloques = bloques;
    this->salidas = salidas;
    this->compuertas = compuertas;
    this->contadorMovimientos = contadorMovimientos;
    this->padre = padre;
    this->movimientoOrigen = Movimiento();

    int alto = staticData->altoTablero;
    int ancho = staticData->anchoTablero;
    this->cuadricula = new char*[alto];
    for(int i = 0; i < alto; i++){
        this->cuadricula[i] = new char[ancho];
    }
    reconstruirCuadricula();
}

//clonar el tablero actual
Tablero* Tablero::clonar(){
    //copiar bloques vivos
    Bloque** nuevosBloques = new Bloque*[this->cantidadBloques];
    for(int i = 0; i < this->cantidadBloques; i++){
        Bloque* orig = this->bloques[i];//crea el arreglo
        nuevosBloques[i] = new Bloque(orig->id, orig->x, orig->y, this->staticData);//agrega cada bloque
    }

    //copiar salidas (todas, aunque no haya bloques adentro siguen existiendo)
    int numSalidas = this->staticData->cantidadSalidas;
    Salida** nuevasSalidas = new Salida*[numSalidas];
    for(int i = 0; i < numSalidas; i++){
        Salida* orig = this->salidas[i];
        Salida* copia = new Salida(orig->id, orig->la, this->staticData);
        copia->contadorpasos = orig->contadorpasos;
        copia->direccion = orig->direccion;
        nuevasSalidas[i] = copia;
    }

    //copiar compuertas
    int numCompuertas = this->staticData->cantidadCompuertas;
    Compuerta** nuevasCompuertas = new Compuerta*[numCompuertas];
    for(int i = 0; i < numCompuertas; i++){
        Compuerta* orig = this->compuertas[i];
        Compuerta* copia = new Compuerta(orig->id, orig->coloractual, this->staticData);
        copia->contadorpasos = orig->contadorpasos;
        nuevasCompuertas[i] = copia;
    }

    //crear nuevo tablero con this como padre
    Tablero* clon = new Tablero(
        this->staticData,
        this->cantidadBloques,
        nuevosBloques,
        nuevasSalidas,
        nuevasCompuertas,
        this->contadorMovimientos,
        this);
    return clon;
}

//retorna el indice del bloque con ese id o -1 si no existe
int Tablero::buscarBloque(uint8_t idBloque){
    for(int i = 0; i < this->cantidadBloques; i++){
        if((uint8_t)this->bloques[i]->id == idBloque){
            return i;
        }
    }
    return -1;
}

bool Tablero::comprobarMovimiento(uint8_t idBloque, char direccion){
    //buscar el indice del bloque
    int idx = buscarBloque(idBloque);
    //si no existe no se valida el movimiento
    if(idx == -1) return false;

    //obtener el puntero al bloque
    Bloque* bloque = this->bloques[idx];
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

            char contenido = this->cuadricula[fila][col];
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


bool Tablero::intentarSalida(uint8_t idBloque){
    //buscar el indice del bloque
    int idx = buscarBloque(idBloque);
    //si no existe no se puede salir
    if(idx == -1) return false;

    //guardar los datos del bloque
    Bloque* bloque = this->bloques[idx];
    int id = bloque->id;
    int anchoB = this->staticData->anchoBloques[id];
    int altoB = this->staticData->altoBloques[id];
    char colorB = this->staticData->coloresBloques[id];

    //ver todas las salidas
    for(int s = 0; s < this->staticData->cantidadSalidas; s++){
        //color debe coincidir
        if(this->staticData->coloresSalidas[s] != colorB) continue;

        //guardar datos de la salida
        int xs = this->staticData->xSalidas[s];
        int ys = this->staticData->ySalidas[s];
        char orient = this->staticData->orientacionSalidas[s];
        int largoActual = this->salidas[s]->la;

        //casos segun orientacion y lado del tablero
        //salida vertical (V) en la pared derecha o izquierda
        if(orient == 'V'){
            //la salida ocupa filas desde ys hasta ys+largoActual-1, en columna xs
            //bloque a la derecha saliendo: borde derecho del bloque adyacente a xs
            //bloque a la izquierda saliendo: borde izquierdo del bloque adyacente a xs
            bool adyDerecha = (bloque->x + anchoB == xs);
            bool adyIzquierda = (bloque->x - 1 == xs);
            if(!adyDerecha && !adyIzquierda){
                continue;
            }
            //el alto del bloque debe ser <= largoActual
            if(altoB > largoActual) {
                continue;
            }
            //las filas del bloque (y..y+altoB-1) deben estar DENTRO del rango de la salida
            if(bloque->y < ys){
                continue;
            }
            if(bloque->y + altoB > ys + largoActual){
                continue;
            }

            //todo ok: el bloque sale
            //eliminar el bloque del arreglo
            delete this->bloques[idx];
            for(int k = idx; k < this->cantidadBloques - 1; k++){
                this->bloques[k] = this->bloques[k + 1];
            }
            this->cantidadBloques--;
            return true;
        }
        //salida horizontal (H) en la pared arriba o abajo
        else {
            //la salida ocupa columnas xs..xs+largoActual-1, en fila ys
            bool adyAbajo = (bloque->y + altoB == ys);
            bool adyArriba = (bloque->y - 1 == ys);
            if(!adyAbajo && !adyArriba){
                continue;
            }

            //si el bloque no cabe, sigue
            if(anchoB > largoActual) {
                continue;
            }

            //comprobar que el bloque este entre xs y xs+largoActual-1
            if(bloque->x < xs) {
                continue;
            }
            if(bloque->x + anchoB > xs + largoActual) {
                continue;
            }

            //todo ok: el bloque sale
            //eliminar el bloque del arreglo
            delete this->bloques[idx];
            for(int k = idx; k < this->cantidadBloques - 1; k++){
                this->bloques[k] = this->bloques[k + 1];
            }
            this->cantidadBloques--;
            return true;
        }
    }
    return false;
}

bool Tablero::intentarCompuerta(uint8_t idBloque){
    //obtener el indice del bloque en el arreglo
    int idx = buscarBloque(idBloque);
    //si el bloque no existe, no puede cruzar compuerta
    if(idx == -1) return false;

    //guardar los datos del bloque
    Bloque* bloque = this->bloques[idx];
    int id = bloque->id;
    int anchoB = this->staticData->anchoBloques[id];
    int altoB = this->staticData->altoBloques[id];
    char colorB = this->staticData->coloresBloques[id];
    uint8_t* geom = this->staticData->geometriaBloques[id];

    //guardar datos del tablero
    int alto = this->staticData->altoTablero;
    int ancho = this->staticData->anchoTablero;

    //comprobar cada compuerta
    for(int c = 0; c < this->staticData->cantidadCompuertas; c++){
        //color de la compuerta debe coincidir con el bloque
        if(this->compuertas[c]->coloractual != colorB){ 
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
                if(fila < 0 || fila >= alto || col < 0 || col >= ancho){
                    hayEspacio = false; break;
                }
                //comprobar que la celda esté vacía
                if(this->cuadricula[fila][col] != ' '){
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

bool Tablero::moverBloque(uint8_t idBloque, char direccion){
    //verifica si se puede hacer el movimiento
    if(!comprobarMovimiento(idBloque, direccion)){
         return false;
    }
    //buscar el indice del bloque a mover
    int idx = buscarBloque(idBloque);
    //guardar el puntero del bloque a mover
    Bloque* bloque = this->bloques[idx];

    //aplicar el desplazamiento de 1 celda
    if(direccion == 'U') bloque->y -= 1;
    else if(direccion == 'D') bloque->y += 1;
    else if(direccion == 'L') bloque->x -= 1;
    else if(direccion == 'R') bloque->x += 1;

    
    //actualizar compuertas y salidas según su contador de pasos
    for(int c = 0; c < this->staticData->cantidadCompuertas; c++){
        this->compuertas[c]->actualizarCompuerta(
            this->staticData->colorICompuertas[c],
            this->staticData->colorFCompuertas[c],
            this->staticData->pasosCompuertas[c]);
    }
    for(int s = 0; s < this->staticData->cantidadSalidas; s++){
        this->salidas[s]->actualizarSalida(
            this->staticData->largoISalidas[s],
            this->staticData->largoFSalidas[s],
            this->staticData->pasosSalidas[s]);
    }
    //avanzar el tiempo: 1 paso por celda deslizada
    this->contadorMovimientos += 1;

    //registrar el movimiento
    this->movimientoOrigen = Movimiento(idBloque, direccion, 1);
    
    //reconstruir cuadricula con la nueva posición + tiempo avanzado
    reconstruirCuadricula();

    //ahora chequear eventos post-movimiento (sin costo adicional)
    if(intentarSalida(idBloque)){
        reconstruirCuadricula(); //se eliminó el bloque
        return true;
    }
    if(intentarCompuerta(idBloque)){
        reconstruirCuadricula(); //cambió la posición
        return true;
    }

    return true;
}

//Libera el arreglo de cuadricula para ahorrar memoria.
//Usado por A* al mover tableros al ClosedSet: alli solo nos interesa
//comparar estados por hash/esIgual y seguir punteros padre, no ver la visual.
//Si alguien despues llama imprimir() o moverBloque() sobre este tablero,
//reconstruirCuadricula() la regenerara automaticamente.
void Tablero::liberarCuadricula(){
    if(this->cuadricula == nullptr) return;
    int alto = this->staticData->altoTablero;
    for(int i = 0; i < alto; i++){
        delete[] this->cuadricula[i];
    }
    delete[] this->cuadricula;
    this->cuadricula = nullptr;
}