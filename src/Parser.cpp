#include <iostream>
#include <fstream>
#include <string>
#include "Parser.h"

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
    std::ifstream archivo(path);
    
    if (!archivo.is_open()) {
        std::cout << "Can't read the lvl" << std::endl;
        return 1;
    }
}