#pragma once
#include "StaticData.h"

//Holds the dynamic data of the Exit (id, actualLen,stepsCount, direction)
class Exit{
    public:
    //atributos
    int8_t id;
    int8_t actualLen;// tamaño actual de la salida
    int8_t stepsCount;// cuantos espacios va a variar su tamaño entre li y lf
    // la direccion siempre empieza en 0 (indica que le toca crecer) y cambia a 1 (indica que le toca achicar)
    char direction;// indica si a la salida le toca crecer o achicar
    StaticData* staticData;

    //constructor
    Exit(int8_t id,
        int8_t actualLen,
        StaticData* staticData);

    void updateExit();
};