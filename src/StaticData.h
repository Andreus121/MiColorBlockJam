#pragma once
#include <cstdint>

class StaticData{
    public:
    //datos del tablero
    int anchoTablero;
    int altoTablero;
    char** paredesTablero;
    int limiteMovimientos;

    //datos de cada bloque
    //el id de cada bloque será el mismo que el indice de su respectivo dato
    int cantidadBloques;
    char* coloresBloques;
    int* altoBloques;
    int* anchoBloques;
    uint8_t** geometriaBloques;

    //datos de las salidas
    //el id de cada salida será el mismo que el indice de su respectivo dato
    int cantidadSalidas;
    int* xSalidas;
    int* ySalidas;
    char* coloresSalidas;
    char* orientacionSalidas;
    int* largoISalidas;
    int* largoFSalidas;
    int* pasosSalidas;

    //datos de las compuertas
    //el id de cada compuerta será el mismo que el indice de su respectivo dato
    int cantidadCompuertas;
    int* xCompuertas;
    int* yCompuertas;
    char* colorICompuertas;
    char* colorFCompuertas;
    int* pasosCompuertas;

    //constructor
    StaticData();
};