
#ifndef CONTR_H
#define CONTR_H
#include <Arduino.h>

static boolean pumpStateOnWell = false; // This is the state of the well pump, true if it is on, false if it is off
static boolean pumpStateOnSys = false;  // This is the state of the system pump, true if it is on, false if it is off

#define PSI_TO_BAR_CONST_shiftL24 17351225 // 0.0689475729*2^24=17 351 224.85
#define PSI_TO_BAR_CONST 0.0689475729

word mapVoltToPSI(word p);
float PSItoBar(word p);

void wellPumpOff();
boolean wellPumpOn();
void systemPumpOff();
boolean systemPumpOn();

#endif // CONTR_H