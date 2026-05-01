#include <string>
#include "Salida.h"

Salida::Salida(int id,
        int la,
        StaticData* staticData){
    this->id = id;
    this->la = la;
    this->direccion = 0;
    this->contadorpasos = 1;
    this->staticData = staticData;
}

//el cambio de tamaño es de rebote, si llega al largo maximo cambia de direccion para volver hasta largo inicial
//li: largo inicial, lf: largo final, pasos: cada cuantos pasos se cambia el tamaño de la salida
void Salida::actualizarSalida(int li, int lf, int pasos){
    // si la salida es de tamaño fijo, no se actualiza
    if(pasos == 0){
        return;
    }

    // si la salida llega a su largo máximo, cambia la dirección a achicar
    if(this->la == lf){
        this->direccion = 1;
    }
    // si es igual a su largo inicial (minimo), cambia la dirección a crecer
    else if(this->la == li){
        this->direccion = 0;
    }
    
    // actualiza el tamaño de la salida
    // segun corresponda si crece o achica
    if(this->contadorpasos == pasos){
        this->contadorpasos = 1;
        if(this->direccion == 0){
            this->la += 1;
        } else {
            this->la -= 1;
        }
    }else{ // si no debe cambiar el tamaño aumenta cuantos pasos lleva con el mismo tamaño
        this->contadorpasos++;
    }
}