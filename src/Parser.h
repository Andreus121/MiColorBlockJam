#pragma once
#include "StaticData.h"
#include "Bloque.h"
#include "Salida.h"
#include "Compuerta.h"
 
/*
Parser lee un archivo de configuración de Color Block Jam y construye
los datos estáticos (StaticData) y los datos dinámicos iniciales
(arreglos de Bloque*, Salida*, Compuerta*) listos para entregar
al constructor de Tablero.

Formato esperado (ver enunciado):
[META]
NAME, WIDTH, HEIGHT, STEP_LIMIT

[BLOCK]
ID COLOR=c WIDTH=w HEIGHT=h INIT_X=x INIT_Y=y GEOMETRY=...

[WALL]
matriz de '#' y ' ' del tamaño WIDTH x HEIGHT

[EXIT]
OLOR=c X=x Y=y ORIENTATION=H|V LI=li LF=lf STEP=p

[GATE]
COLOR=c X=x Y=y ORIENTATION=H|V LI=li CI=ci CF=cf STEP=p

Convención: X es columna, Y es fila. Internamente cuadricula[fila][col].
*/
class Parser {
    public:
    //datos parseados (rellenados por cargar())
    StaticData* staticData;
    Bloque** bloques;
    Salida** salidas;
    Compuerta** compuertas;
 
    Parser();
    ~Parser();
 
    //lee el archivo en 'ruta'. Retorna true si se cargó bien, false en error.
    //Tras cargar, los atributos staticData/bloques/salidas/compuertas
    //quedan poblados y listos para usarse.
    bool cargar(const char* ruta);
};