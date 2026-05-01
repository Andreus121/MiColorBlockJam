#include <iostream>
#include <fstream>
#include <cstring>
#include "Parser.h"

Parser::Parser(){
    this->staticData = nullptr;
    this->bloques = nullptr;
    this->salidas = nullptr;
    this->compuertas = nullptr;
}

Parser::~Parser(){
    //el Parser NO libera staticData/bloques/salidas/compuertas
    //esos punteros se entregan al Tablero, que es su nuevo dueño.
}


//helpers locales (funciones auxiliares dentro del .cpp, no contaminan el header)
//¿La línea está vacía o solo tiene espacios?
static bool lineaVacia(const char* s){
    for(int i = 0; s[i] != '\0'; i++){
        if(s[i] != ' ' && s[i] != '\t' && s[i] != '\r' && s[i] != '\n') return false;
    }
    return true;
}

//Busca "KEY" seguido de '=' (con o sin espacios alrededor) en la línea.
//Retorna puntero al primer caracter del valor (después del '=' y posibles espacios).
//Si no la encuentra, retorna nullptr.
//Importante: requiere que justo ANTES de KEY haya inicio de línea o un caracter
//no alfanumérico, para no confundir "WIDTH" con "INIT_WIDTH" (no existe pero por
//las dudas) ni con un sufijo de otra clave.
static const char* buscarValor(const char* linea, const char* key){
    int klen = std::strlen(key);
    int llen = std::strlen(linea);
    for(int i = 0; i + klen <= llen; i++){
        //chequeo de borde izquierdo: inicio o caracter no alfanumérico/_
        if(i > 0){
            char prev = linea[i - 1];
            bool esAlfa = (prev >= 'A' && prev <= 'Z') || (prev >= 'a' && prev <= 'z')
                       || (prev >= '0' && prev <= '9') || prev == '_';
            if(esAlfa) continue;
        }
        if(std::strncmp(linea + i, key, klen) != 0) continue;
        //después del key permitimos espacios opcionales y luego '='
        int j = i + klen;
        //mientras haya espacios o tabs avanza
        while(j < llen && (linea[j] == ' ' || linea[j] == '\t')){
            j++;
        }
        if(j >= llen || linea[j] != '='){
            continue;
        }
        j++; //pasar el '='
        while(j < llen && (linea[j] == ' ' || linea[j] == '\t')){
            j++;
        }
        return linea + j;
    }
    return nullptr;
}

//Lee un entero a partir del puntero 's'. Avanza el puntero hasta después del número.
static int leerInt(const char*& s){
    //saltar espacios al inicio
    while(*s == ' ' || *s == '\t') s++;
    int signo = 1;
    if(*s == '-'){ signo = -1; s++; }
    int v = 0;
    while(*s >= '0' && *s <= '9'){
        v = v * 10 + (*s - '0');
        s++;
    }
    return signo * v;
}

//Lee un caracter no-espacio y avanza el puntero.
static char leerChar(const char*& s){
    //saltar espacios y tabs al inicio
    while(*s == ' ' || *s == '\t'){
        s++;
    }
    char c = *s;
    if(c != '\0') s++;
    return c;
}

//comenzar la carga de datos
bool Parser::cargar(const char* ruta){
    std::ifstream f(ruta);
    if(!f.is_open()){
        std::cout << "Error: no se pudo abrir " << ruta << std::endl;
        return false;
    }

    //creamos el StaticData y vamos rellenándolo
    this->staticData = new StaticData();
    StaticData* sd = this->staticData;

    //buffer para línea actual
    char linea[1024];

    //pasada 1: contar cuántos bloques, salidas, compuertas para reservar arreglos
    int countBloques = 0, countSalidas = 0, countCompuertas = 0;
    int wallStart = -1, wallEnd = -1; //líneas de inicio y fin del bloque [WALL]
    int seccion = 0; //0=meta, 1=block, 2=wall, 3=exit, 4=gate
    int numLinea = 0;
    while(f.getline(linea, sizeof(linea))){
        numLinea++;
        if(linea[0] == '['){
            if(std::strncmp(linea, "[META]", 6) == 0) seccion = 0;
            else if(std::strncmp(linea, "[BLOCK]", 7) == 0) seccion = 1;
            else if(std::strncmp(linea, "[WALL]", 6) == 0) seccion = 2;
            else if(std::strncmp(linea, "[EXIT]", 6) == 0) seccion = 3;
            else if(std::strncmp(linea, "[GATE]", 6) == 0) seccion = 4;
            continue;
        }
        //si la linea es vacia saltala
        if(lineaVacia(linea)){
            continue;
        }
        //si estamos en la seccion de bloques, suma 1 bloque
        if(seccion == 1){
            countBloques++;
        }
        //si estamos en la seccion de paredes, actualiza el inicio y final de las paredes
        else if(seccion == 2){
            if(wallStart == -1){
                wallStart = numLinea;
            }
            wallEnd = numLinea;
        }
        //contar salidas
        else if(seccion == 3){
            countSalidas++;
        }
        //contar compuertas
        else if(seccion == 4){
            countCompuertas++;
        }
    }

    //guardar cantidades en el StaticData
    sd->cantidadBloques = countBloques;
    sd->cantidadSalidas = countSalidas;
    sd->cantidadCompuertas = countCompuertas;

    //reservar arreglos
    if(countBloques > 0){
        sd->coloresBloques = new char[countBloques];
        sd->anchoBloques = new int[countBloques];
        sd->altoBloques = new int[countBloques];
        sd->geometriaBloques = new uint8_t*[countBloques];
        for(int i = 0; i < countBloques; i++) sd->geometriaBloques[i] = nullptr;
    }
    if(countSalidas > 0){
        sd->xSalidas = new int[countSalidas];
        sd->ySalidas = new int[countSalidas];
        sd->coloresSalidas = new char[countSalidas];
        sd->orientacionSalidas = new char[countSalidas];
        sd->largoISalidas = new int[countSalidas];
        sd->largoFSalidas = new int[countSalidas];
        sd->pasosSalidas = new int[countSalidas];
    }
    if(countCompuertas > 0){
        sd->xCompuertas = new int[countCompuertas];
        sd->yCompuertas = new int[countCompuertas];
        sd->colorICompuertas = new char[countCompuertas];
        sd->colorFCompuertas = new char[countCompuertas];
        sd->pasosCompuertas = new int[countCompuertas];
    }

    //arreglos para los datos dinámicos iniciales
    Bloque** bloques = (countBloques > 0) ? new Bloque*[countBloques] : nullptr;
    int* initX = (countBloques > 0) ? new int[countBloques] : nullptr;
    int* initY = (countBloques > 0) ? new int[countBloques] : nullptr;
    Salida** salidas = (countSalidas > 0) ? new Salida*[countSalidas] : nullptr;
    Compuerta** compuertas = (countCompuertas > 0) ? new Compuerta*[countCompuertas] : nullptr;

    //pasada 2: rellenar todo
    f.clear();
    f.seekg(0);
    seccion = 0;
    int idxBloque = 0, idxSalida = 0, idxCompuerta = 0;
    int wallRow = 0; //fila actual del [WALL] que estamos leyendo

    while(f.getline(linea, sizeof(linea))){
        if(linea[0] == '['){
            if(std::strncmp(linea, "[META]", 6) == 0) seccion = 0;
            else if(std::strncmp(linea, "[BLOCK]", 7) == 0) seccion = 1;
            else if(std::strncmp(linea, "[WALL]", 6) == 0){
                seccion = 2;
                //al llegar a [WALL] ya conocemos altoTablero (en META)
                //reservar paredesTablero
                sd->paredesTablero = new char*[sd->altoTablero];
                for(int i = 0; i < sd->altoTablero; i++){
                    sd->paredesTablero[i] = new char[sd->anchoTablero];
                    for(int j = 0; j < sd->anchoTablero; j++){
                        sd->paredesTablero[i][j] = ' ';
                    }
                }
                wallRow = 0;
            }
            else if(std::strncmp(linea, "[EXIT]", 6) == 0) seccion = 3;
            else if(std::strncmp(linea, "[GATE]", 6) == 0) seccion = 4;
            continue;
        }

        //sección WALL es especial: copiamos los chars literalmente
        if(seccion == 2){
            if(wallRow >= sd->altoTablero) continue;
            //copiar hasta anchoTablero, completar con espacios si la línea es corta
            for(int j = 0; j < sd->anchoTablero; j++){
                char c = linea[j];
                if(c == '\0' || c == '\n' || c == '\r') c = ' ';
                sd->paredesTablero[wallRow][j] = c;
            }
            wallRow++;
            continue;
        }

        if(lineaVacia(linea)) continue;

        if(seccion == 0){
            //META: NAME, WIDTH, HEIGHT, STEP_LIMIT
            const char* v;
            if((v = buscarValor(linea, "WIDTH"))) sd->anchoTablero = leerInt(v);
            else if((v = buscarValor(linea, "HEIGHT"))) sd->altoTablero = leerInt(v);
            else if((v = buscarValor(linea, "STEP_LIMIT"))) sd->limiteMovimientos = leerInt(v);
            //NAME se ignora por ahora; podríamos guardarlo si lo necesitas
            continue;
        }

        if(seccion == 1){
            //BLOCK: "ID COLOR=c WIDTH=w HEIGHT=h INIT_X=x INIT_Y=y GEOMETRY=..."
            const char* p = linea;
            int idArchivo = leerInt(p); //ID en el archivo (1-indexed según ejemplo)
            //usamos idxBloque (0-indexed) como id interno; guardamos el mapeo implícito por orden
            (void)idArchivo;

            const char* v;
            char color = '?';
            int w = 0, h = 0, ix = 0, iy = 0;
            if((v = buscarValor(linea, "COLOR"))) color = leerChar(v);
            if((v = buscarValor(linea, "WIDTH"))) w = leerInt(v);
            if((v = buscarValor(linea, "HEIGHT"))) h = leerInt(v);
            if((v = buscarValor(linea, "INIT_X"))) ix = leerInt(v);
            if((v = buscarValor(linea, "INIT_Y"))) iy = leerInt(v);

            sd->coloresBloques[idxBloque] = color;
            sd->anchoBloques[idxBloque] = w;
            sd->altoBloques[idxBloque] = h;

            //GEOMETRY: w*h enteros separados por espacios
            uint8_t* geom = new uint8_t[w * h];
            for(int k = 0; k < w * h; k++) geom[k] = 0;
            if((v = buscarValor(linea, "GEOMETRY"))){
                for(int k = 0; k < w * h; k++){
                    int g = leerInt(v);
                    geom[k] = (uint8_t)(g != 0 ? 1 : 0);
                }
            }
            sd->geometriaBloques[idxBloque] = geom;

            initX[idxBloque] = ix;
            initY[idxBloque] = iy;
            idxBloque++;
            continue;
        }

        if(seccion == 3){
            //EXIT: COLOR=c X=x Y=y ORIENTATION=H|V LI=li LF=lf STEP=p
            const char* v;
            char color = '?', orient = 'H';
            int x = 0, y = 0, li = 0, lf = 0, step = 0;
            if((v = buscarValor(linea, "COLOR"))) color = leerChar(v);
            if((v = buscarValor(linea, "X"))) x = leerInt(v);
            if((v = buscarValor(linea, "Y"))) y = leerInt(v);
            if((v = buscarValor(linea, "ORIENTATION"))) orient = leerChar(v);
            if((v = buscarValor(linea, "LI"))) li = leerInt(v);
            if((v = buscarValor(linea, "LF"))) lf = leerInt(v);
            if((v = buscarValor(linea, "STEP"))) step = leerInt(v);

            sd->xSalidas[idxSalida] = x;
            sd->ySalidas[idxSalida] = y;
            sd->coloresSalidas[idxSalida] = color;
            sd->orientacionSalidas[idxSalida] = orient;
            sd->largoISalidas[idxSalida] = li;
            sd->largoFSalidas[idxSalida] = lf;
            sd->pasosSalidas[idxSalida] = step;
            idxSalida++;
            continue;
        }

        if(seccion == 4){
            //GATE: COLOR=c X=x Y=y ORIENTATION=H|V LI=li CI=ci CF=cf STEP=p
            //(LI no aplica para compuertas en nuestro modelo, lo ignoramos)
            //COLOR podría no estar; usamos CI/CF
            const char* v;
            char ci = '?', cf = '?';
            int x = 0, y = 0, step = 0;
            if((v = buscarValor(linea, "X"))) x = leerInt(v);
            if((v = buscarValor(linea, "Y"))) y = leerInt(v);
            if((v = buscarValor(linea, "CI"))) ci = leerChar(v);
            if((v = buscarValor(linea, "CF"))) cf = leerChar(v);
            if((v = buscarValor(linea, "STEP"))) step = leerInt(v);

            sd->xCompuertas[idxCompuerta] = x;
            sd->yCompuertas[idxCompuerta] = y;
            sd->colorICompuertas[idxCompuerta] = ci;
            sd->colorFCompuertas[idxCompuerta] = cf;
            sd->pasosCompuertas[idxCompuerta] = step;
            idxCompuerta++;
            continue;
        }
    }

    f.close();

    //crear los Bloque/Salida/Compuerta dinámicos iniciales
    for(int i = 0; i < countBloques; i++){
        bloques[i] = new Bloque((uint8_t)i, initX[i], initY[i], sd);
    }
    for(int i = 0; i < countSalidas; i++){
        salidas[i] = new Salida(i, sd->largoISalidas[i], sd);
    }
    for(int i = 0; i < countCompuertas; i++){
        compuertas[i] = new Compuerta(i, sd->colorICompuertas[i], sd);
    }

    //limpiar buffers temporales
    if(initX) delete[] initX;
    if(initY) delete[] initY;

    //asignar al estado del Parser
    this->bloques = bloques;
    this->salidas = salidas;
    this->compuertas = compuertas;

    return true;
}