#pragma once
#include <string>
#include "StaticData.h"

/*
Si una pieza toca una compuerta del mismo color, la pieza
aparece del otro lado de la compuerta y la compuerta varia el color con el timepo
sin importar si una pieza lo atraviesa o no,
este tiempo puede ser de 1, 2 o más pasos. si es 0 el color es fijo
*/
class Compuerta {
    public:
    //atributos
    int id;
    char coloractual;
    int contadorpasos;
    StaticData* staticData;

    Compuerta(int id,
        char coloractual,
        StaticData* staticData);

    void actualizarCompuerta(char colorInicial, char colorFinal, int pasos);
};