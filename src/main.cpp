/**
 * @file main.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-12-03
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "contr.h"
#include "defs.h"
#include "ui.h"
#include <Arduino.h>

const unsigned long intervalLed = (2UL * 1000);

// Set LED_BUILTIN if it is not defined by Arduino framework
// #define LED_BUILTIN 13

unsigned long previousTime = 0;    // Variable to store the previous time
unsigned long previousTimeLed = 0; // Variable to store the previous time
#define serial_speed 38400

void setup()
{
    // initialize LED digital pin as an output.
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.begin(serial_speed);
    while (!Serial) {
        ; // wait for serial port to connect. Needed for native USB port only
    }
    Serial.println();
    Serial.println(F("Serial connected"));
    // Serial.print(F("Board type: "));
    Serial.print(F("Serial speed is: "));
    Serial.println(serial_speed);
    // Serial.println(F(""));
    setupOLed();
    // Serial.print(F("testing PSI map, 'minValue': "));
    // Serial.print(mapVoltToPSI(minValue));
    // Serial.print(F(", 'minValue-1': "));
    // Serial.print(mapVoltToPSI(minValue - 1));
    // Serial.print(F(", 'minValue+1': "));
    // Serial.print(mapVoltToPSI(minValue + 1));
    // Serial.print(F(", 'minValue + VoltToPIS + 1': "));
    // Serial.println(mapVoltToPSI(minValue + VoltToPIS + 1));

    // Serial.print(F(", 'maxValue': "));
    // Serial.print(mapVoltToPSI(maxValue));
    // Serial.print(F(", 'maxValue-1': "));
    // Serial.print(mapVoltToPSI(maxValue - 1));
    // Serial.print(F(", 'maxValue+1': "));
    // Serial.print(mapVoltToPSI(maxValue + 1));
    // Serial.print(F(", 'maxValue + VoltToPIS + 1': "));
    // Serial.print(mapVoltToPSI(maxValue + VoltToPIS + 1));
    // Serial.print(F(", 'maxValue - VoltToPIS - 1': "));
    // Serial.println(mapVoltToPSI((maxValue - VoltToPIS) - 1));

    // delay(20000);
    previousTimeLed = millis() - intervalLed;
    pinMode(PinPSI, INPUT);
    pinMode(PinPump1, OUTPUT);
    pinMode(PinPump2, OUTPUT);
    // Serial.print(F("Wait for "));Serial.print(minTOnTime / 1000);
    // Serial.println(F(" seconds"));
    // delay(minTOnTime);// The pumps can't be turn on immediately after booting to prevent rapid cycling if the board is constantly power cycling.
    // Serial.println(F("Turning pumps on for 15 seconds to test relays"));
    // wellPumpOn();
    // systemPumpOn();
    // // digitalWrite(PinPump1, HIGH);
    // digitalWrite(PinPump2, HIGH);
    // delay(15000); // wait for pumps to turn on
    Serial.println(F("turning pumps off "));
    wellPumpOff();
    systemPumpOff();
    pinMode(lostPrimeLed, OUTPUT);
    // Serial.println(F("Pump should be off here"));
    // delay(10000);
    Serial.println(F("leaving setup"));
}

byte c = 0;
word loopCount = 0;
byte lastPSI = 123;
void loop()
{
    static boolean oneM1 = false; // just print the error once until the error is cleared
    static boolean t1 = false;
    // TEST: testing PCB board relays
    // static byte ledLaststate = LOW;
    // if (ledLaststate == HIGH) {
    //     wellPumpOn();
    //     systemPumpOff();
    // } else {
    //     wellPumpOff();
    //     systemPumpOn();
    // }
    // ledLaststate = !ledLaststate;
    // digitalWrite(lostPrimeLed, ledLaststate);
    // delay(1000); // wait for 1 second before toggling again
    // END TEST

    loopCount++;
    unsigned long currentTime = millis(); // Get the current time in milliseconds, 1 second = 1000 milliseconds
    int16_t sensorValue, tSV;
    byte sensorPSI;
    tSV = analogRead(PinPSI);
    if (tSV < minValue) {
        sensorValue = tSV;
    } else if (tSV > (sensorValue + (valuePerPSI/2))) {
        sensorValue = tSV;
    } else if ((tSV + (valuePerPSI/2)) < sensorValue) {
        sensorValue = tSV;
    }
    // sensorValue = tSV;
    sensorPSI = mapVoltToPSI(sensorValue);

    // If the PSI Changes update the display
    if ((sensorPSI != lastPSI) and (sensorValue > ((minValue) / 4))) {
        lastPSI = sensorPSI;
        oledWriteAt(sensorPSI, 6, 1, 4, 2);
        float bar;
        bar = PSItoBar(sensorPSI);
        oledWriteAt(bar, 6, 3, 4, 1);
        // Serial.print(F("Pump pressure is: "));
        // Serial.print(sensorPSI);
        // Serial.print(F(", sensorValue: "));
        // Serial.println(sensorValue);
    }
    // Serial.print(F("Line number: "));
    // Serial.print(__LINE__);
    // Serial.print(F(" time: "));
    // Serial.println(millis() - currentTime);
    /// the minimum value for a working a connected sensor should be 0.5V so if it
    /// is less than 1/4 of that we should have a problem. The pin than the sensor
    /// is on should be pulled low to detect such problems as a disconnected
    /// sensor os bad conenction.
    if (oneM1) {
        if (sensorValue >= ((minValue) / 4)) {
            oneM1 = false;
            Serial.print(F("sensorValue is now >= to 1/4 of it's minimum value: "));
            Serial.println(sensorValue);
            OLedPSINoError();
        }
    }
    if (sensorValue < ((minValue) / 4)) {
        systemPumpOff();
        if (!oneM1) {
            oneM1 = true;
            printPSIError(sensorValue);
            OLedPSIError();
            ledBlink(LCErrorSensor);
        }
    } else if (sensorPSI > maxPSI) {
        if (pumpStateOnSys == true) {
            Serial.print(F("Pump pressure at turn off: "));
            Serial.print(sensorPSI);
            Serial.print(F(", Turn off pressure is: "));
            Serial.print(maxPSI);
            Serial.print(F(", sensorValue is: "));
            Serial.println(sensorValue);
        }
        systemPumpOff();
        // Serial.printf(F("Debug: At line: " CURRENT_LINE " pumpOff();"));
        ledBlink(LCUpToPressure);
    } else if ((sensorPSI < minPSI) and (pumpStateOnSys != true)) {
        if (!systemPumpOn()) {
            if (!t1) {
                t1 = true;
                Serial.print(F("Not tuning on at pressure of: "));
                Serial.print(sensorPSI);
                Serial.print(F(" as not been "));
                Serial.print(minTOnTime / 1000);
                Serial.println(F(" seconds since last turn on,"));
            }
            delay(1000);
        } else {
            Serial.print(F("System pump state changed to: "));
            Serial.println(pumpStateOnSys);
            ledBlink(LCPumpOn);
            Serial.print(F("Turn on pressure: "));
            Serial.println(minPSI);
            t1 = false;
        }
    }
    // Serial.print(F("Line number: "));
    // Serial.print(__LINE__);
    // Serial.print(F(" time: "));
    // Serial.println(millis() - currentTime);

    if (currentTime - previousTimeLed >= intervalLed) { // 2000 milliseconds = 2 seconds
        previousTimeLed = currentTime;
        ledBlink();
        if (c >= 5) {
            Serial.print(F("Pump pressure is: "));
            Serial.print(sensorPSI);
            Serial.print(F(", Top float: "));
            if (topFloatIsWater) {
                Serial.print(F("water"));
            } else {
                Serial.print(F("no water"));
            }
            Serial.print(F(", bottom float: "));
            if (lowerFloatIsWater) {
                Serial.print(F("water, "));
            } else {
                Serial.print(F("no water, "));
            }
            Serial.println();
            // Serial.print(F(", Loop count "));
            // Serial.println(loopCount);
            // sensorPSI = 18;
            oledWriteAt(sensorPSI, 6, 1, 3, 2);
            float bar;
            bar = PSItoBar(sensorPSI);
            oledWriteAt(bar, 6, 3, 4, 1);
            c = 0;
            loopCount = 0;
        }
        c++;
    }
    checkTankFloats();
    // Serial.print(F("Line number: "));
    // Serial.print(__LINE__);
    // Serial.print(F(" time: "));
    // Serial.println(millis() - currentTime);
    delay(500); // wait for 0.5 seconds before the next loop iteration
}