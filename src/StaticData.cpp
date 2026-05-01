#include "StaticData.h"

StaticData::StaticData(){
    //datos del tablero
    this->anchoTablero = 0;
    this->altoTablero = 0;
    this->paredesTablero = nullptr;

    //datos de cada bloque
    this->cantidadBloques = 0;
    this->coloresBloques = nullptr;
    this->altoBloques = nullptr;
    this->anchoBloques = nullptr;
    this->geometriaBloques = nullptr;

    //datos de las salidas
    this->cantidadSalidas = 0;
    this->xSalidas = nullptr;
    this->ySalidas = nullptr;
    this->coloresSalidas = nullptr;
    this->orientacionSalidas = nullptr;
    this->largoISalidas = nullptr;
    this->largoFSalidas = nullptr;
    this->pasosSalidas = nullptr;

    //datos de las compuertas
    this->cantidadCompuertas = 0;
    this->xCompuertas = nullptr;
    this->yCompuertas = nullptr;
    this->colorICompuertas = nullptr;
    this->colorFCompuertas = nullptr;
    this->pasosCompuertas = nullptr;
}