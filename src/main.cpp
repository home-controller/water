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
    previousTime = millis() - interval;
    pinMode(PinPSI, INPUT);
    pinMode(PinPump1, OUTPUT);
    pinMode(PinPump2, OUTPUT);
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
    // TEST: testing PCB board relays
    static byte ledLaststate = LOW;
    if (ledLaststate == HIGH) {
        wellPumpOn();
        systemPumpOff();
    } else {
        wellPumpOff();
        systemPumpOn();
    }
    ledLaststate = !ledLaststate;
    digitalWrite(lostPrimeLed, ledLaststate);

    loopCount++;
    unsigned long currentTime = millis(); // Get the current time
    word sensorValue = analogRead(PinPSI);
    byte sensorPSI = mapVoltToPSI(sensorValue);
    /// the minimum value for a working a connected sensor should be 0.5V so if it
    /// is less than 1/4 of that we should have a problem. The pin than the sensor
    /// is on should be pulled low to detect such problems as a disconnected
    /// sensor os bad conenction.
    if (sensorValue < ((minValue) / 4)) {
        systemPumpOff();
        // Serial.printf(F("Debug: At line: " CURRENT_LINE " pumpOff();"));
        previousTime = currentTime;
        // oledWriteAt(sensorPSI, 6, 1, 3, 2);

        Serial.print(F("sensorValue is less than 1/4 of it's minimum value: "));
        Serial.println(sensorValue);
        Serial.println(F("Likely disconnected sensor or bad connection"));
        Serial.print(F("Curent Pump Borehole state: "));
        Serial.print(PumpStateOn);
        Serial.print(F(", Pin D4: "));
        Serial.print(digitalRead(4));
        //
        OLedPSIError();
        ledBlink(LCErrorSensor);
    } else if (sensorPSI > maxPSI) {
        if (PumpStateOn == true) {
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
    } else if ((sensorPSI < minPSI) and (PumpStateOn != true)) {
        if (currentTime - previousTime >= interval) {
            // 60 seconds have passed, execute your code here

            systemPumpOn();
            // Serial.printf(F("Debug: At line: " CURRENT_LINE " pumpOn();"));
            previousTime = currentTime; // Update the previous time
            ledBlink(LCPumpOn);
            Serial.print(F("Pressure to turn on at: "));
            Serial.println(minPSI);

        } else {
            Serial.print(F("Not tuning on at pressure of: "));
            Serial.print(sensorPSI);
            Serial.print(F(" as only been: "));
            Serial.print((currentTime - previousTime) / 1000);
            Serial.println(F(" seconds since last turn on,"));
            delay(5000);
        }
    }
    if (currentTime - previousTimeLed >= intervalLed) {
        previousTimeLed = currentTime;
        ledBlink();
        if (c >= 5) {
            Serial.print(F("Pump pressure is: "));
            Serial.print(sensorPSI);
            Serial.print(F(", Loop count "));
            Serial.println(loopCount);
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
}