#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
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
    std::string line;
    staticData->blockCount = 0;
    staticData->exitCount = 0;
    staticData->gateCount = 0;
    while (std::getline(file, line)) {
        //if is in the [META] seccion = 0, [BLOCK] = 1,
        //[WALL] = 2, [EXIT] = 3, [GATE] = 4
        int8_t seccion = 0;
        if (line == "[META]") {
            seccion = 0;
        } else if (line == "[BLOCK]") {
            seccion = 1;
        } else if (line == "[WALL]") {
            seccion = 2;
        } else if (line == "[EXIT]") {
            seccion = 3;
        } else if (line == "[GATE]") {
            seccion = 4;
        }
        
        //read/count lines of the seccion
        std::stringstream ss(line);
        std::string trash;
        if(seccion == 0){//[META]
            ss >> trash >> trash; //NAME lvlname
            ss >> trash >> staticData->boardWidth; //WIDTH number
            ss >> trash >> staticData->boardHeight; //HEIGHT number
            ss >> trash >> staticData->stepLimit; //SETP_LIMIT number
            ss >> trash;
        }
        if(seccion == 1){//[BLOCKS]
            while(!line.empty()){
                staticData->blockCount += 1;
            }
        }
        if(seccion == 2){//[WALL]
            //save the grid with walls
            staticData->boardWalls[staticData->boardWidth];
            for(int8_t i = 0; i < staticData->boardWidth; i++){
                ss >> staticData->boardWalls[i];
            }
            
        }

}