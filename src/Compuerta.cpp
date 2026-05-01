#include <iostream>
#include <string>
#include "Compuerta.h"

Compuerta::Compuerta(int id,
        char coloractual,
        StaticData* staticData){
    this->id=id;
    this->coloractual=coloractual;
    this->contadorpasos=1;
    this->staticData=staticData;
}

// el cambio de color es ciclico, o sea que si llega al color final y al limite de pasos
// debe volver al color inicial
void Compuerta::actualizarCompuerta(char colorInicial, char colorFinal, int pasos){
    // si la compuerta es de color fijo, no se actualiza
    if (pasos==0){
        return;
    }
    // cambia el color segun corresponda si aumenta o disminuye
    if (this->contadorpasos==pasos){
        this->contadorpasos=1;
        //vuelve al color inicial si llega al final
        if(this->coloractual==colorFinal){
            this->coloractual=colorInicial;
        } else { // si no llega al final debe seguir avanzando
            this->coloractual++;
        }  
    } else { // si no debe cambiar el color aumenta cuantos pasos lleva con el mismo color
        this->contadorpasos++;
    }
}