#include "Arduino.h"

// Set LED_BUILTIN if it is not defined by Arduino framework
// #define LED_BUILTIN 13

unsigned long previousTime = 0;                // Variable to store the previous time
unsigned long previousTimeLed = 0;             // Variable to store the previous time
const unsigned long interval = (60UL * 1000);  // 30 seconds in milliseconds
const unsigned long intervalLed = (2UL * 1000);
static boolean PumpStateOn = true;
;

#define pumpOnV LOW
#define serial_speed 38400

// Analog read pin for pump presser in psi.
#define PinPSI A1
#define PinPump 2

// Input 5V Output 0.5-4.5V / 0-5V(0-100PSI)
byte minPSI = 20;
byte maxPSI = 30;
#define MCUVolt 5
#define analogReadMaxRes 1024  // MAX RESOLUTION for nano, 10 bits or 0..1024
#define resPerVolt 205         // 204.8
#define minValue 102           // (1024/5) x 0.5 =102
#define maxValue 922           // 205 x 4.5 = 922.5
#define maxRange 820           // This could vary a bit.
#define sensorMaxPSI 75
#define VoltToPIS 820 / sensorMaxPSI  // depends on sensor. For 75psi max VoltToPIS =

word mapVoltToPSI(word p) {
    if (p > maxValue) { p = maxValue; }
    if (p < minValue) { p = minValue; }
    return map(p, minValue, maxValue, 0, sensorMaxPSI);
}

void pumpOff() {
    if (PumpStateOn == true) {
#if (pumpOnV == HIGH)
        digitalWrite(PinPump, LOW);
#else
        digitalWrite(PinPump, HIGH);
#endif
        PumpStateOn = false;
        Serial.println(F("Turned pump off"));
    }
}

void pumpOn() {
    if (PumpStateOn != true) {
#if (pumpOnV == HIGH)
        digitalWrite(PinPump, HIGH);
#else
        digitalWrite(PinPump, LOW);
#endif
        PumpStateOn = true;
        Serial.println(F("Turned pump on"));
    }
}

#define LCPumpOn 1
#define LCUpToPressure 2
#define LCErrorLostPrime 2
#define LCErrorExpansionTank 3

void ledBlink(byte state = 0) {
    static byte blinkCode = 0;
    byte i;

    if ((state > 0) and (state != blinkCode)) {
        blinkCode = state;
    } else {
        state = blinkCode;
    }
    if (state == 0) {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(3000);
        digitalWrite(LED_BUILTIN, LOW);
        delay(3000);
    } else if (state == LCPumpOn) {
        digitalWrite(LED_BUILTIN, HIGH);
    } else if (state == LCUpToPressure) {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(1000);
        digitalWrite(LED_BUILTIN, LOW);
    } else if (state == LCErrorLostPrime) {
        for (i = 0; i < 6; i++) {
            digitalWrite(LED_BUILTIN, HIGH);
            delay(500);
            digitalWrite(LED_BUILTIN, LOW);
            delay(500);
        }
        delay(2000);
    } else if (state == LCErrorLostPrime) {
        for (i = 0; i < 4; i++) {
            digitalWrite(LED_BUILTIN, HIGH);
            delay(500);
            digitalWrite(LED_BUILTIN, LOW);
            delay(1000);
        }
        delay(2000);
    }
}
void setup() {
    // initialize LED digital pin as an output.
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.begin(serial_speed);
    while (!Serial) {
        ;  // wait for serial port to connect. Needed for native USB port only
    }
    Serial.println();
    Serial.println(F("Serial connected"));
    // Serial.print(F("Board type: "));
    Serial.print(F("Serial speed is: "));
    Serial.println(serial_speed);
    // Serial.println(F(""));

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
    pumpOff();
    pinMode(PinPump, OUTPUT);
    // Serial.println(F("Pump should be off here"));
    // delay(10000);
    Serial.println(F("leaving setup"));
}
byte c = 0;
void loop() {
    unsigned long currentTime = millis();  // Get the current time
    word sensorValue = analogRead(PinPSI);
    byte sensorPSI = mapVoltToPSI(sensorValue);
    if (sensorPSI > maxPSI) {
        if (PumpStateOn == true) {
            Serial.print(F("Pump pressure at urn off: "));
            Serial.print(sensorPSI);
            Serial.print(F(", Turn off pressure is: "));
            Serial.print(maxPSI);
            Serial.print(F(", sensorValue is: "));
            Serial.println(sensorValue);
        }
        pumpOff();
        ledBlink(LCUpToPressure);
    }
    if ((sensorPSI < minPSI) and (PumpStateOn != true)) {
        if (currentTime - previousTime >= interval) {
            // 60 seconds have passed, execute your code here

            pumpOn();
            previousTime = currentTime;  // Update the previous time
            ledBlink(LCPumpOn);
            Serial.print(F("Pressure at turn on: "));
            Serial.println(sensorPSI);
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
            Serial.println(sensorPSI);
            c = 0;
        }
        c++;
    }
}