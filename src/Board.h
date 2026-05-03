#pragma once
#include <iostream>
#include <string>
#include "StaticData.h"
#include "Block.h"
#include "Exit.h"
#include "Gate.h"

/*
Board representa el estado completo del juego en un instante dado.
Esta clase es la que el algoritmo A* usa como "state": se clona por cada
vecino generado, guardando solo datos dinámicos (lo estático vive en StaticData*).
Cada Board conoce a su padre (el estado del que vino) y qué movimiento
lo creó, para poder reconstruir el camino solución al final.
*/
class Board{
    public:
    //puntero a datos estaticos
    StaticData* staticData;

    //datos dinamicos
    int8_t blocksCount; //bloques en juego
    Block** blocks;  //arreglo de punteros a bloques en juego
    Exit** exits;  //arreglo de punteros a salidas en juego
    Gate** gates; //arreglo de punteros a compuertas en juego
    uint8_t movesCount; //contador de movimientos realizados en el turno actual (inicia en 0)

    char** grid;
    
    //constructor
    Board(StaticData* staticData,
        Block** bloques,
        Exit** salidas,
        Gate** compuertas,
        uint8_t movesCount=0,
        int8_t blocksCount=0);

    //destructor: libera los bloques, salidas, compuertas y la cuadrícula
    ~Board();

    //rebuild the Grid with all of his characters
    void rebuildGrid();

    void printGrid();

    bool isSolved();
    
    //clone all data of the Table except the grid
    Board* clone();    

    int8_t findBlock(int8_t idBloque);

    bool canMove(int8_t idBloque, char direccion);

    bool moveBlock(int8_t idBloque, char direccion);

    bool tryExit(int8_t idBloque);

    bool tryGate(int8_t idBloque);
};