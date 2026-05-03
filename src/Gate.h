#pragma once
#include <string>
#include "StaticData.h"

//Holds the dynamic data of the Gate (id, actualColor, stepsCount)
class Gate{
    public:
    int8_t id;
    int8_t actualColor;//index of the color staticData->gateColorSequences
    int8_t stepsCount;
    StaticData* staticData;

    Gate(int8_t id,
        int8_t actualColor,
        StaticData* staticData);
    
    //change the stepsCount, and change actualColor if stepsCount=stepsLimit
    void updateGate();
};