#include "Arduino.h"

// Set LED_BUILTIN if it is not defined by Arduino framework
// #define LED_BUILTIN 13

unsigned long previousTime = 0;                // Variable to store the previous time
const unsigned long interval = (60UL * 1000);  // 30 seconds in milliseconds

// Analog read pin for pump presser in psi.
#define PinPSI A1
#define PinPump 2

// Input 5V Output 0.5-4.5V / 0-5V(0-100PSI)
byte minPSI = 15;
byte maxPSI = 30;
#define MCUVolt 5
#define analogReadMaxRes 1024  // MAX RESOLUTION for nano, 10 bits or 0..1024
#define resPerVolt 205         // 204.8
#define minValue 102           // (1024/5) x 0.5 =102
#define maxValue 922           // 205 x 4.5 = 922.5
#define maxRange 820           // This could vary a bit.
#define sensorMaxPSI 75
#define VoltToPIS 820 / sensorMaxPSI  // depends on sensor

word mapVoltToPSI(word p) {
    if (p > maxValue) { p = maxValue; }
    return map(p, minValue, maxValue, 0, sensorMaxPSI);
}

void pumpOff() {
    digitalWrite(PinPump, LOW);
}
void pumpOn() {
    digitalWrite(PinPump, LOW);
}

void setup() {
    // initialize LED digital pin as an output.
    pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
    // turn the LED on (HIGH is the voltage level)
    digitalWrite(LED_BUILTIN, HIGH);

    delay(1000);                     // wait for a second
    digitalWrite(LED_BUILTIN, LOW);  // turn the LED off by making the voltage LOW
    delay(1000);                     // wait for a second

    unsigned long currentTime = millis();  // Get the current time
    word sensorValue = analogRead(PinPSI);
    byte sensorPSI = mapVoltToPSI(sensorValue);
    if (sensorPSI > maxPSI) {
        pumpOff();
    }
    if (currentTime - previousTime >= interval) {
        // 60 seconds have passed, execute your code here
        if (sensorPSI < minPSI) {
            pumpOn();
        }

        previousTime = currentTime;  // Update the previous time
    }
}