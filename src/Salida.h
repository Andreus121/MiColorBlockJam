#pragma once
#include <iostream>
#include <string>
#include "StaticData.h"
/*
Si una pieza toca una salida del mismo color, la pieza desaparece si el tamaño coincide
además que las salidas pueden variar su tamaño con el tiempo, manteniendo su tamaño durante 0, 1 o más pasos
*/
class Salida{
    public:
    //atributos
    int id;
    int la;// tamaño actual de la salida
    int contadorpasos;// cuantos espacios va a variar su tamaño entre li y lf
    // la direccion siempre empieza en 0 (indica que le toca crecer) y cambia a 1 (indica que le toca achicar)
    char direccion;// indica si a la salida le toca crecer o achicar
    StaticData* staticData;

    //constructor
    Salida(int id,
        int la,
        StaticData* staticData);

    void actualizarSalida(int li, int lf, int pasos);
};