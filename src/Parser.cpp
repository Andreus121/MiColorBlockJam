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
    staticData->exitColors = new char[staticData->exitCount];
    staticData->exitOrientations = new char[staticData->exitCount];
    staticData->exitInitialLenghts = new int8_t[staticData->exitCount];
    staticData->exitFinalLenghts = new int8_t[staticData->exitCount];
    staticData->exitSteps = new int8_t[staticData->exitCount];
    
    //this part was made for Claude sonnet 4.6

    //reserve memory for gates
    staticData->gateX = new int8_t[staticData->gateCount];
    staticData->gateY = new int8_t[staticData->gateCount];
    staticData->gateOrientations = new char[staticData->gateCount];
    staticData->gateLengths = new int8_t[staticData->gateCount];
    staticData->gateColorSequenceLengths = new int8_t[staticData->gateCount];
    staticData->gateColorSequences = new char*[staticData->gateCount];
    staticData->gateSteps = new int8_t[staticData->gateCount];
    for (int i = 0; i < staticData->gateCount; i++){
        staticData->gateColorSequences[i] = nullptr;
    }
    
    int8_t* initX = new int8_t[staticData->blockCount];
    int8_t* initY = new int8_t[staticData->blockCount];
 
    // --- PASS 2: read blocks, exits, gates ---
    file.clear();
    file.seekg(0);
    section = 0;
    int blockIdx = 0, exitIdx = 0, gateIdx = 0;
 
    while (file.getline(buffer, MAX_LINE)) {
 
        if (buffer[0] == '[') {
            if      (std::strcmp(buffer, "[META]")  == 0) section = 0;
            else if (std::strcmp(buffer, "[BLOCK]") == 0) section = 1;
            else if (std::strcmp(buffer, "[WALL]")  == 0) section = 2;
            else if (std::strcmp(buffer, "[EXIT]")  == 0) section = 3;
            else if (std::strcmp(buffer, "[GATE]")  == 0) section = 4;
            continue;
        }
 
        // skip empty lines and wall section entirely
        if (section == 2) continue;
        if (buffer[0] == '\0') continue;
 
        const char* p = buffer;
 
        if (section == 1) {
            skipWord(p); // skip "COLOR"
            staticData->blockColors[blockIdx] = readChar(p);
            skipWord(p); // skip "WIDTH"
            staticData->blockWidth[blockIdx]  = (int8_t)readInt(p);
            skipWord(p); // skip "HEIGHT"
            staticData->blockHeight[blockIdx] = (int8_t)readInt(p);
            int8_t w = staticData->blockWidth[blockIdx];
            int8_t h = staticData->blockHeight[blockIdx];
            skipWord(p); // skip "INIT_X"
            initX[blockIdx] = (int8_t)readInt(p);
            skipWord(p); // skip "INIT_Y"
            initY[blockIdx] = (int8_t)readInt(p);
            skipWord(p); // skip "GEOMETRY"
            staticData->blockGeometrics[blockIdx] = new uint8_t[w * h];
            for (int k = 0; k < w * h; k++) {
                staticData->blockGeometrics[blockIdx][k] = (uint8_t)readInt(p);
            }
            blockIdx++;
        }
        else if (section == 3) {
            skipWord(p); // skip "COLOR"
            staticData->exitColors[exitIdx]         = readChar(p);
            skipWord(p); // skip "X"
            staticData->exitX[exitIdx]              = (int8_t)readInt(p);
            skipWord(p); // skip "Y"
            staticData->exitY[exitIdx]              = (int8_t)readInt(p);
            skipWord(p); // skip "ORIENTATION"
            staticData->exitOrientations[exitIdx]   = readChar(p);
            skipWord(p); // skip "LI"
            staticData->exitInitialLenghts[exitIdx] = (int8_t)readInt(p);
            skipWord(p); // skip "LF"
            staticData->exitFinalLenghts[exitIdx]   = (int8_t)readInt(p);
            skipWord(p); // skip "STEP"
            staticData->exitSteps[exitIdx]          = (int8_t)readInt(p);
            exitIdx++;
        }
        else if (section == 4) {
            // format: X 13 Y 7 ORIENTATION V LI 4 COLORS a b c d _ STEP 1
            skipWord(p); // skip "X"
            staticData->gateX[gateIdx]            = (int8_t)readInt(p);
            skipWord(p); // skip "Y"
            staticData->gateY[gateIdx]            = (int8_t)readInt(p);
            skipWord(p); // skip "ORIENTATION"
            staticData->gateOrientations[gateIdx] = readChar(p);
            skipWord(p); // skip "LI"
            staticData->gateLengths[gateIdx]      = (int8_t)readInt(p);
            skipWord(p); // skip "COLORS"
 
            // read colors until '_' terminator
            int8_t seqLen = 0;
            char tempColors[32];
            char c;
            while ((c = readChar(p)) != '_' && c != '\0' && seqLen < 32) {
                tempColors[seqLen++] = c;
            }
            staticData->gateColorSequences[gateIdx] = new char[seqLen];
            for (int k = 0; k < seqLen; k++) {
                staticData->gateColorSequences[gateIdx][k] = tempColors[k];
            }
            staticData->gateColorSequenceLengths[gateIdx] = seqLen;
 
            skipWord(p); // skip "STEP"
            staticData->gateSteps[gateIdx] = (int8_t)readInt(p);
            gateIdx++;
        }
    }
 
    // --- create dynamic objects ---
    blocks = new Block*[staticData->blockCount];
    exits  = new Exit*[staticData->exitCount];
    gates  = new Gate*[staticData->gateCount];
 
    for (int i = 0; i < staticData->blockCount; i++) {
        blocks[i] = new Block((int8_t)i, initX[i], initY[i], staticData);
    }
    for (int i = 0; i < staticData->exitCount; i++) {
        exits[i] = new Exit(i, staticData->exitInitialLenghts[i], staticData);
    }
    for (int i = 0; i < staticData->gateCount; i++) {
        gates[i] = new Gate(i, staticData->gateColorSequences[i][0], staticData);
    }
 
    delete[] initX;
    delete[] initY;
 
    return 0;
}