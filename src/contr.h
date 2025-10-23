
#ifndef CONTR_H
#define CONTR_H
#include <Arduino.h>

extern boolean pumpStateOnWell; // This is the state of the well pump, true if it is on, false if it is off
extern boolean pumpStateOnSys;

extern boolean topFloatIsWater;   // This is the state of the top float, true if it has water, false if no water
extern boolean lowerFloatIsWater; // This is the state of the lower float, true if has water, false if no water
//extern boolean aFloatChanged; // This is true if a float has changed since the last check
//extern boolean updateTankPump; // This is true if the tank pump needs to be updated i.e. turned on or off
//extern unsigned long lastFloatChangeTime; // This is the last time a float changed state

#define PSI_TO_BAR_CONST_shiftL24 17351225 // 0.0689475729*2^24=17 351 224.85
#define PSI_TO_BAR_CONST 0.0689475729

word mapVoltToPSI(word p);
float PSItoBar(word p);

void checkTankFloats();

void wellPumpOff();
boolean wellPumpOn();
void systemPumpOff();
boolean systemPumpOn();

#endif // CONTR_H