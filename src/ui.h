#ifndef UI_H
#define UI_H

#include <Arduino.h>

#define LINE_STRINGIZE(x) #x
#define LINE_VALUE_TO_STRING(x) LINE_STRINGIZE(x)
#define CURRENT_LINE LINE_VALUE_TO_STRING(__LINE__)

// for OLED display ---------------------------------------------
#include <Adafruit_SH110X.h>

#define i2c_Address 0x3c // initialize with the I2C addr 0x3C Typically eBay OLED's
#define OLED_RESET -1    //   QT-PY / XIAO
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

void ledBlink(byte state = 0);

byte oledWriteAt(byte n, byte x_char, byte y_line, byte padTo = 0, byte textSize = 1, byte charWidth = 6, byte charHight = 8);
byte oledWriteAt(float n, byte x_char, byte y_line, byte padTo = 3, byte textSize = 1, byte charWidth = 6, byte charHight = 8);

void setupOLed();
void OLedPSIError();
void printPSIError(word sensorValue);

#endif // UI_H
