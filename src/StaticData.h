#pragma once
#include <cstdint>

//Holds only the STATIC data of the rest of pieces
class StaticData{
    public:
    //board's data
    uint8_t boardWith; //ancho
    uint8_t BoardHeight; //largo
    char** boardWalls;
    uint16_t stepLimit;

    //block's data
    uint8_t blockCount;
    char* blockColors;
    uint8_t* blockWidth;
    uint8_t* blockHeight;
    uint8_t** blockGeometrics;

    //exit's data
    uint8_t exitCount;
    int8_t* exitX;
    int8_t* exitY;
    char* exitColors;
    char* exitOrientations; //V or H (vertical or horizontal)
    uint8_t* exitInitialLenghts; 
    uint8_t* exitFinalLenghts;
    uint8_t* exitSteps;

    //gate's data
    uint8_t gateCount;
    int8_t* gateX;
    int8_t* gateY;
    uint8_t* gateColorSequenceLengths;
    char* gateColorSequences;
    uint8_t* gateSteps;
    /*
    uint8_t = 0 -> 255
    uint16_t = 0 -> 65535
    int8_t = -128 -> 127
    */

    //constructor
    StaticData();
};