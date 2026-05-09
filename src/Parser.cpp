#include <iostream>
#include <fstream>
#include <cstring>
#include "Parser.h"
//max line length for reading the file
#define MAX_LINE 1024

Parser::Parser(){
    this->staticData = nullptr;
    this->blocks = nullptr;
    this->exits = nullptr;
    this->gates = nullptr;
}

//dont free anything
Parser::~Parser(){}

//auxiliar functions
static void skipSpaces(const char*& p){
    //if the characates is spaces just sum 1 to the pointer
    while(*p == ' ' || *p == '\t') p++;
}

static int readInt(const char*& p){
    skipSpaces(p);
    int sign = 1;
    if(*p == '-'){
        sign = -1;
        p++;
    }
    int number = 0;
    while(*p >= '0' && *p <= '9'){
        number = number * 10 + (*p-'0');
        p++;
    }
    return sign * number;
}

static char readChar(const char*& p){
    skipSpaces(p);
    char c = *p;
    if(c != '\0') p++;
    return c;
}

static void skipWord(const char*& p){
    skipSpaces(p);
    while(*p!= ' ' && *p != '\t' && *p!='\0') p++;
}

//read and save the data of a lvl
int8_t Parser::save(const char* path){

    //create the object
    std::ifstream file(path);
    
    if (!file.is_open()) {
        std::cout << "Can't read the lvl" << std::endl;
        return 1;
    }

    //prepare memory for the StaticData
    this->staticData = new StaticData();
    char buffer[MAX_LINE]; //array of characters for the line
    staticData->blockCount = 0;
    staticData->exitCount = 0;
    staticData->gateCount = 0;
    int8_t section = 0;
    int8_t wallRow = 0;
    //count blocks, exits and gates
    while (file.getline(buffer,MAX_LINE)) {
        //if is in the [META] section = 0, [BLOCK] = 1,
        //[WALL] = 2, [EXIT] = 3, [GATE] = 4
        if (buffer[0] == '['){//new section
            if(std::strcmp(buffer, "[META]")==0) section = 0;
            else if(std::strcmp(buffer, "[BLOCK]")==0) section = 1;
            else if(std::strcmp(buffer, "[WALL]")==0){
                section = 2;
                //save memory for the grid of walls
                staticData->boardWalls = new char*[staticData->boardHeight];
                for (int i = 0; i < staticData->boardHeight; i++) {
                    staticData->boardWalls[i] = new char[staticData->boardWidth];
                }
                wallRow = 0;
            }
            else if(std::strcmp(buffer, "[EXIT]")==0) section = 3;
            else if(std::strcmp(buffer, "[GATE]")==0) section = 4;
            continue;
        }
        
        //skip empty lines out of the section [WALL]
        if (section != 2 && buffer[0] == '\0') continue;
        
        //read/count lines of the section
        const char* p = buffer;
        if(section == 0){//[META]
            skipWord(p); //skip "NAME"
            skipWord(p); //skip name lvl
            skipWord(p); //skip "WIDTH"
            staticData->boardWidth = readInt(p);
            skipWord(p);  //skip "HEIGHT"
            staticData->boardHeight = readInt(p);
            skipWord(p);  //skip  "STEP_LIMIT"
            staticData->stepLimit = readInt(p);
        }
        if(section == 1){//[BLOCKS]
            staticData->blockCount += 1;
        }
        else if(section == 2){//[WALL]
            for (int i = 0; i < staticData->boardWidth; i++) {
                char c = buffer[i];
                if (c == '\0' || c == '\r') c = ' ';
                staticData->boardWalls[wallRow][i] = c;
            }
            wallRow++;
        }
        else if(section == 3){//[EXIT]
            staticData->exitCount += 1;
        }
        else if(section == 4){//[Gate]
            staticData->gateCount += 1;
        }
    }

    //reserve memory for blocks
    staticData->blockColors = new char[staticData->blockCount];
    staticData->blockWidth = new int8_t[staticData->blockCount];
    staticData->blockHeight = new int8_t[staticData->blockCount];
    staticData->blockGeometrics = new uint8_t*[staticData->blockCount];

    //reserve memory for exits
    staticData->exitX = new int8_t[staticData->exitCount];
    staticData->exitY = new int8_t[staticData->exitCount];
    staticData->exitColors = new char[staticData->blockCount];
    staticData->exitOrientations = new char[staticData->blockCount];
    staticData->exitInitialLenghts = new int8_t[staticData->exitCount];
    staticData->exitFinalLenghts = new int8_t[staticData->exitCount];
    staticData->exitSteps = new int8_t[staticData->exitCount];
    
    //reserve memory for gates
    staticData->gateX = new int8_t[staticData->gateCount];
    staticData->gateY = new int8_t[staticData->gateCount];
    staticData->gateColorSequenceLengths = new int8_t[staticData->gateCount];
    staticData->gateColorSequences = new char*[staticData->gateCount];
    staticData->gateSteps = new int8_t[staticData->gateCount];

    //reset section and file
    file.clear();
    file.seekg(0);
    section = 0;
    //save blocks, exits and gates
    while (file.getline(buffer,MAX_LINE)){


    }
}