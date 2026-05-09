#pragma once
#include "StaticData.h"
#include "Block.h"
#include "Exit.h"
#include "Gate.h"
 
/*
Read a file.txt and create the StaticData, Blocks, Exits and Gates

Format:
[META]
NAME, WIDTH, HEIGHT, STEP_LIMIT
[BLOCK]
COLOR=c WIDTH=w HEIGHT=h INIT_X=x INIT_Y=y GEOMETRY=...
[WALL]
matriz de '#' y ' ' del tamaño WIDTH x HEIGHT
[EXIT]
OLOR=c X=x Y=y ORIENTATION=H|V LI=li LF=lf STEP=p
[GATE]
X=x Y=y ORIENTATION=H|V LI=li CI=ci CF=cf STEP=p
*/
class Parser{
    public:
    //data
    StaticData* staticData;
    Block** blocks;
    Exit** exits;
    Gate** gates;
 
    Parser();
    ~Parser();
 
    //read the file "path". Return 0 if its ok, else 1.
    //holds the data readed in the a Parser
    int8_t save(const char* path);

    //auxiliar functions
    private:
    static void skipSpaces(const char*& p);

    static int readInt(const char*& p);
};