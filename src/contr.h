
#ifndef CONTR_H
#define CONTR_H
#include <Arduino.h>

static boolean PumpStateOn = true;

#define PSI_TO_BAR_CONST_shiftL24 17351225 // 0.0689475729*2^24=17 351 224.85
#define PSI_TO_BAR_CONST 0.0689475729

word
mapVoltToPSI(word p);
void
pumpOff();
void
pumpOn();
float
PSItoBar(word p);

#endif // CONTR_H