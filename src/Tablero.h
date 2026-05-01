#pragma once
#include <iostream>
#include <string>
#include "StaticData.h"
#include "Bloque.h"
#include "Salida.h"
#include "Compuerta.h"

/*
Tablero representa el estado completo del juego en un instante dado.
Esta clase es la que el algoritmo A* usa como "state": se clona por cada
vecino generado, guardando solo datos dinámicos (lo estático vive en StaticData*).
Cada Tablero conoce a su padre (el estado del que vino) y qué movimiento
lo creó, para poder reconstruir el camino solución al final.
*/
class Tablero {
    public:
    //puntero a datos estaticos
    StaticData* staticData;

    //datos dinamicos
    int cantidadBloques; //bloques en juego
    Bloque** bloques;  //arreglo de punteros a bloques en juego
    Salida** salidas;  //arreglo de punteros a salidas en juego
    Compuerta** compuertas; //arreglo de punteros a compuertas en juego
    int contadorMovimientos; //contador de movimientos realizados en el turno actual (inicia en 0)
    
    //datos para reconstruir el camino solución
    Tablero* padre; //puntero al tablero del que se vino

    char** cuadricula;
    
    //constructor
    Tablero(StaticData* staticData,
        Bloque** bloques,
        Salida** salidas,
        Compuerta** compuertas);

    //destructor: libera los bloques, salidas, compuertas y la cuadrícula
    ~Tablero();

    //reconstruye cuadricula desde paredes + bloques + salidas + compuertas
    void reconstruirCuadricula();

    //imprime la cuadrícula actual en consola
    void imprimir();

    //todos los bloques han salido?
    bool esSolucion();

    //constructor usado para clonar el tablero en A*
    Tablero(StaticData* staticData,
        int cantidadBloques,
        Bloque** bloques,
        Salida** salidas,
        Compuerta** compuertas,
        int contadorMovimientos,
        Tablero* padre);

    //crea una copia profunda de este tablero.
    //el clon tiene al original como padre.
    Tablero* clonar();    

    //busca el bloque con ese id en el arreglo de bloques vivos.
    //retorna índice en el arreglo, o -1 si no está.
    int buscarBloque(uint8_t idBloque);

    //chequea si el bloque idBloque puede moverse 1 celda en esa direccion
    //sin chocar con paredes, otros bloques o salidas apagadas.
    //direccion: 'U','D','L','R'.
    bool comprobarMovimiento(uint8_t idBloque, char direccion);

    //aplica un movimiento de 1 celda. Si tras el movimiento el bloque
    //queda adyacente a una salida o compuerta válida, despues del movimiento
    //se llama a intentarSalida() o intentarCompuerta() para ver si el bloque sale o cruza la compuerta.
    //sin costo al usar la salida o compuerta, además
    //suma 1 a contadorMovimientos. Retorna true si el movimiento se hizo.
    bool moverBloque(uint8_t idBloque, char direccion);

    //después de un movimiento, chequea si el bloque puede salir.
    //si sí, elimina el bloque del arreglo y retorna true.
    bool intentarSalida(uint8_t idBloque);

    //después de un movimiento, chequea si el bloque puede cruzar
    //una compuerta adyacente. si sí, teletransporta y retorna true.
    bool intentarCompuerta(uint8_t idBloque);
};