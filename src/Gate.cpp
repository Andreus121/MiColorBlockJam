#include <string>
#include "Gate.h"

Gate::Gate(int8_t id,
        int8_t actualColor,
        StaticData* staticData){
    this->id=id;
    this->actualColor=actualColor;
    this->stepsCount=1;
    this->staticData=staticData;
}

//change te color based in the stepsCount and stepsLimit
void Gate::updateGate(){
    int8_t colorSequencesLen = staticData->gateColorSequenceLengths[id];
    char* colorSequences = staticData->gateColorSequences[id];
    int8_t stepsLimit = staticData->gateSteps[id];

    //if the color its static, dont change anything
    if (stepsLimit=0) return;
    //change the color if its the final steps
    if(stepsCount==stepsLimit){
        stepsCount=1;
        //if is the final color, change to the first color
        if(colorSequences[actualColor]==colorSequences[colorSequencesLen-1]){
            actualColor=0;
        } 
        else actualColor++;//if is not the final color, pass to the next color
    }
    //if not need change color, sum 1 to steps
    else stepsCount++;
}