#include <string>
#include "Exit.h"

Exit::Exit(int8_t id,
        int8_t actualLen,
        StaticData* staticData){
    this->id = id;
    this->actualLen = actualLen;
    this->direction = 0;
    this->stepsCount = 1;
    this->staticData = staticData;
}

//update the len, direction and steps of the exit.
//the change of colors in the exit its a "come and back"
//initialLen, finalLen, limitSteps
void Exit::updateData(){

    int8_t initialLen = staticData->exitInitialLenghts[id];
    int8_t finalLen = staticData->exitFinalLenghts[id];
    int8_t stepsLimit = staticData->exitSteps[id];

    // if the len its static dont change anything
    if(stepsLimit == 0) return;

    // if the len is max, change the direction to decrease
    if(actualLen == finalLen) direction = 1;

    // if the len is min, change the direction to increase
    else if(actualLen == initialLen) direction = 0;
    
    // reset the steps and 
    // increase or decrease the value of actualLen 
    // depending of the direction
    if(stepsCount == stepsLimit){
        stepsCount = 1;
        //increase
        if(direction == 0)actualLen += 1;
        //decrease
        else actualLen -= 1;
    }
    // if dont need change the actualLen, only increase the stepsCount
    else stepsCount++;

}