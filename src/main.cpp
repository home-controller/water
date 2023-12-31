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

#include "Arduino.h"

// for OLED display
#include <Adafruit_SH110X.h>

#define i2c_Address 0x3c  // initialize with the I2C addr 0x3C Typically eBay OLED's
#define OLED_RESET -1     //   QT-PY / XIAO
#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

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

#define PinPSI A1  // Analog read pin for pump presser in psi.
#define PinPump1 3  // To relay to turn pump on/off with pressure sensor
#define PinPump1 4  // To relay to turn pump on/off borehole pump to fill tank. Float controlled
#define PinFloat1 5 // Connect to the low float
#define PinFloat2 6 // Connect to the high float
#define PinFlow1  7 // Connect to flow sensor switch, pump to first tank. This is for checking prime or any other problem with borehole pump.
#define PinFlow2  8 // As above but for system pressure pump after settlement tanks.


// Input 5V Output 0.5-4.5V / 0-5V(0-100PSI)
byte minPSI = 20;
byte maxPSI = 35;
#define MCUVolt 5
#define analogReadMaxRes 1024  // MAX RESOLUTION for nano, 10 bits or 0..1024
#define resPerVolt 205         // 204.8
#define minValue 102           // (1024/5) x 0.5 =102
#define maxValue 922           // 205 x 4.5 = 922.5
#define maxRange 820           // This could vary a bit.
#define sensorMaxPSI 80
#define VoltToPIS 820 / sensorMaxPSI  // depends on sensor. For 75psi max VoltToPIS =

word mapVoltToPSI(word p) {
    if (p > maxValue) { p = maxValue; }
    if (p < minValue) { p = minValue; }
    return map(p, minValue, maxValue, 0, sensorMaxPSI);
}

#define PSI_TO_BAR_CONST_shiftL24 17351225  // 0.0689475729*2^24=17 351 224.85
#define PSI_TO_BAR_CONST 0.0689475729

/**
 * @brief Takes the pressure in PSI and returns it in bar with the 4 digits after the decimal point in r.
 *
 * @param p The pressure in PSI
 * @param r the remainder x 10000. i.e. 4 places after the point, 0.xxxx
 * @return word the whole number part of the bars
 */
word PSItoBar(word p, word &r) {
    uint32_t bar;
    bar = PSI_TO_BAR_CONST_shiftL24 * (uint32_t)p;
    bar = bar >> 16;
    r = bar bitand 0xFF;
    r = ((uint32_t)r * 10000) >> 8;
    return (bar >> 8);
}

float PSItoBar(word p) {
    float bar;
    bar = (float)p * (float)PSI_TO_BAR_CONST;
    return bar;
}

void pumpOff() {
    if (PumpStateOn == true) {
#if (pumpOnV == HIGH)
        digitalWrite(PinPump, LOW);
#else
        digitalWrite(PinPump1, HIGH);
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
        digitalWrite(PinPump1, LOW);
#endif
        PumpStateOn = true;
        Serial.println(F("Turned pump on"));
    }
}

#define LCPumpOn 1
#define LCUpToPressure 2
#define LCErrorLostPrime 2
#define LCErrorExpansionTank 3
#define LCErrorSensor 4  // The sensor should have a voltage of 0.5 for 0 PSI. So if less the 0.5 volts there is a problem like disconnected sensor etc.

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
    } else if (state == LCErrorSensor) {
        for (i = 0; i < 4; i++) {
            digitalWrite(LED_BUILTIN, HIGH);
            delay(250);
            digitalWrite(LED_BUILTIN, LOW);
            delay(250);
            digitalWrite(LED_BUILTIN, HIGH);
            delay(250);
            digitalWrite(LED_BUILTIN, LOW);
            delay(250);
            digitalWrite(LED_BUILTIN, HIGH);
            delay(3000);
            digitalWrite(LED_BUILTIN, LOW);
            delay(1000);
        }
        delay(2000);
    }
}

byte oledWriteAt(byte n, byte x_char, byte y_line, byte padTo = 0, byte textSize = 1, byte charWidth = 6, byte charHight = 8) {
    if (x_char < 1 or y_line < 1) return -1;
    if (padTo > 0) {
        display.fillRect((x_char - 1) * textSize * charWidth, (y_line - 1) * textSize * charHight, padTo * textSize * charWidth, 1 * textSize * charHight, SH110X_WHITE);
    }

    display.setTextSize(textSize);
    display.setTextColor(SH110X_WHITE, SH110X_BLACK);
    if ((padTo > 0))
        display.setCursor((x_char - 1) * charWidth * textSize, (y_line - 1) * charHight);
    display.println(n);
    display.display();
    return 0;
}

byte oledWriteAt(float n, byte x_char, byte y_line, byte padTo = 3, byte textSize = 1, byte charWidth = 6, byte charHight = 8) {
    if (x_char < 1 or y_line < 1) return -1;
    byte charL;
    charL = 1;
    if (n > 10) {
        charL++;
        if (n > 100) {
            charL++;
            if (n > 1000) {
                charL++;
                if (n > 10000) {
                    charL++;
                }
            }
        }
    }

    if (padTo > 0) {
        display.fillRect((x_char - 1) * textSize * charWidth, (y_line - 1) * textSize * charHight, (padTo + 1) * textSize * charWidth, 1 * textSize * charHight, SH110X_WHITE);
    }

    display.setTextSize(textSize);
    display.setTextColor(SH110X_WHITE, SH110X_BLACK);
    if ((padTo > 0)) {
        display.setCursor((x_char - 1) * charWidth * textSize, (y_line - 1) * charHight);
        if (charL > padTo) {
            display.print(">");
        } else if (charL == padTo) {
            display.print(n, 0);
        } else {
            display.print(n, padTo - (charL));
        }
    }
    display.display();
    return 0;
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

    display.begin(i2c_Address, true);  // Address 0x3C default
    display.cp437(true);
    // display.setContrast (0); // dim display
    // text display tests
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(0, 0);
    display.println("PSI: ???");
    display.setTextSize(1);
    display.println("BAR: ?.???");
    display.print("PSI Min:");
    display.print(minPSI);
    display.print(" Max:");
    display.println(maxPSI);
    display.println("Tank: bellow low");
    display.println("Flow tank: No,  sys: No.");

    // display.setTextSize(3);
    // display.setTextColor(SH110X_BLACK, SH110X_WHITE);  // 'inverted' text
    // display.println(3.141592);
    // display.setTextSize(2);
    // display.setTextColor(SH110X_WHITE);
    // display.print("0x");
    // display.println(0xDEADBEEF, HEX);
    display.display();
    delay(2000);
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
    pumpOff();
    pinMode(PinPump1, OUTPUT);
    // Serial.println(F("Pump should be off here"));
    // delay(10000);
    Serial.println(F("leaving setup"));
}
byte c = 0;
word loopCount = 0;
byte lastPSI = 123;
void loop() {
    loopCount++;
    unsigned long currentTime = millis();  // Get the current time
    word sensorValue = analogRead(PinPSI);
    byte sensorPSI = mapVoltToPSI(sensorValue);
    /// the minimum value for a working a connected sensor should be 0.5V so if it is less than 1/4 of that we should have a problem.
    /// The pin than the sensor is on should be pulled low to detect such problems as a disconnected sensor os bad conenction.
    if (sensorValue < ((minValue) / 4)) {
        pumpOff();
        previousTime = currentTime;
        // oledWriteAt(sensorPSI, 6, 1, 3, 2);
        display.setTextSize(2);
        display.setTextColor(SH110X_WHITE, SH110X_BLACK);
        display.setCursor(0, 0);
        display.println("PSI Error ");
        display.setTextSize(1);
        display.println("Sensor1 disconnected?");
        display.display();
        Serial.print(F("sensorValue is less than 1/4 of it's minimum value: "));
        Serial.println(sensorValue);
        Serial.println(F("Likely disconnected sensor or bad connection"));
        //
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
        pumpOff();
        ledBlink(LCUpToPressure);
    } else if ((sensorPSI < minPSI) and (PumpStateOn != true)) {
        if (currentTime - previousTime >= interval) {
            // 60 seconds have passed, execute your code here

            pumpOn();
            previousTime = currentTime;  // Update the previous time
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