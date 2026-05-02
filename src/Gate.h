#pragma once
#include <string>
#include "StaticData.h"

//Holds the dynamic data of the Gate (id, actualColor, stepsCount)
class Gate{
    public:
    uint8_t id;
    uint8_t actualColor;//index of the color staticData->gateColorSequences
    uint8_t stepsCount;
    StaticData* staticData;

    Gate(uint8_t id,
        uint8_t actualColor,
        StaticData* staticData);
    
    //change the stepsCount, and change actualColor if stepsCount=stepsLimit
    void updateGate();
};