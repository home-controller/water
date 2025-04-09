
#include "contr.h"
#include "defs.h"

word mapVoltToPSI(word p)
{
    if (p > maxValue) { p = maxValue; }
    if (p < minValue) { p = minValue; }
    return map(p, minValue, maxValue, 0, sensorMaxPSI);
}

/**
 * @brief Takes the pressure in PSI and returns it in bar with the 4 digits
 * after the decimal point in r.
 *
 * @param p The pressure in PSI
 * @param r the remainder x 10000. i.e. 4 places after the point, 0.xxxx
 * @return word the whole number part of the bars
 */
word PSItoBar(word p, word& r)
{
    uint32_t bar;
    bar = PSI_TO_BAR_CONST_shiftL24 * (uint32_t)p;
    bar = bar >> 16;
    r = bar bitand 0xFF;
    r = ((uint32_t)r * 10000) >> 8;
    return (bar >> 8);
}

float PSItoBar(word p)
{
    float bar;
    bar = (float)p * (float)PSI_TO_BAR_CONST;
    return bar;
}

void pumpOff()
{
    if (PumpStateOn == true) {
#if (pumpOnV == HIGH)
        digitalWrite(PinPump1, LOW);
#else
        digitalWrite(PinPump1, HIGH);
#endif
        PumpStateOn = false;
        Serial.println(F("Turned pump off"));
    }
}

void pumpOn()
{
    if (PumpStateOn != true) {
#if (pumpOnV == HIGH)
        digitalWrite(PinPump1, HIGH);
#else
        digitalWrite(PinPump1, LOW);
#endif
        PumpStateOn = true;
        Serial.println(F("Turned pump on"));
    }
}
