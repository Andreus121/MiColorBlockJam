#include "StaticData.h"

StaticData::StaticData(){
    //board's data
    this->boardWidth = 0;
    this->boardHeight = 0;
    this->boardWalls = nullptr;
    this->stepLimit = 0;

    //block's data
    this->blockCount = 0;
    this->blockColors = nullptr;
    this->blockWidth = nullptr;
    this->blockHeight = nullptr;
    this->blockGeometrics = nullptr;

    //datos de las salidas
    this->exitCount = 0;
    this->exitX = nullptr;
    this->exitY = nullptr;
    this->exitColors = nullptr;
    this->exitOrientations = nullptr;
    this->exitInitialLenghts = nullptr;
    this->exitFinalLenghts = nullptr;
    this->exitSteps = nullptr;

    //datos de las compuertas
    this->gateCount = 0;
    this->gateX = nullptr;
    this->gateY = nullptr;
    this->gateOrientations = nullptr;
    this->gateLengths = nullptr;
    this->gateColorSequenceLengths = nullptr;
    this->gateColorSequences = nullptr;
    this->gateSteps = nullptr;
}