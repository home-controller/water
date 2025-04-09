#include "ui.h"
#include "defs.h"

Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

/**
 * @brief Controls the blinking pattern of an LED based on the given state.
 *
 * This function manages the LED behavior for various states, including
 * normal operation, pump activation, pressure status, and error conditions.
 * The LED patterns are used to visually indicate the system's status.
 *
 * @param state The state code that determines the LED blinking pattern:
 *              - 0: Default state, LED blinks with a 3-second interval.
 *              - LCPumpOn: LED remains continuously ON.
 *              - LCUpToPressure: LED blinks with a 1-second interval.
 *              - LCErrorLostPrime: LED blinks 6 times with 500ms ON/OFF intervals,
 *                followed by a 2-second pause.
 *              - LCErrorSensor: LED blinks in a complex pattern 4 times, with
 *                short and long intervals, followed by a 2-second pause.
 *
 * The function uses a static variable to retain the last valid state and
 * ensures that the LED behavior corresponds to the current or last valid state.
 *
 * Note: The function assumes the LED is connected to the pin defined by
 *       LED_BUILTIN, and additional pins (4 and 5) are set to HIGH at the start.
 */
void ledBlink(byte state)
{
    static byte blinkCode = 0;
    byte i;
    static byte ledLaststate = LOW;
    digitalWrite(PinPump1, ledLaststate);
    ledLaststate = !ledLaststate;
    digitalWrite(PinPump2, ledLaststate);
    digitalWrite(lostPrimeLed, ledLaststate);
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
    } else if (state == LCErrorLostPrime) { // 8 seconds
        for (i = 0; i < 6; i++) {
            digitalWrite(LED_BUILTIN, HIGH);
            delay(500);
            digitalWrite(LED_BUILTIN, LOW);
            delay(500);
        }
        delay(2000);
    } else if (state == LCErrorSensor) {// 4 x 5 + 2 = 22 seconds
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

byte oledWriteAt(byte n, byte x_char, byte y_line, byte padTo, byte textSize, byte charWidth, byte charHight)
{
    if (x_char < 1 or y_line < 1) return -1;
    if (padTo > 0) {
        display.fillRect(
          (x_char - 1) * textSize * charWidth, (y_line - 1) * textSize * charHight, padTo * textSize * charWidth, 1 * textSize * charHight, SH110X_WHITE);
    }

    display.setTextSize(textSize);
    display.setTextColor(SH110X_WHITE, SH110X_BLACK);
    if ((padTo > 0)) display.setCursor((x_char - 1) * charWidth * textSize, (y_line - 1) * charHight);
    display.println(n);
    display.display();
    return 0;
}

byte oledWriteAt(float n, byte x_char, byte y_line, byte padTo, byte textSize, byte charWidth, byte charHight)
{
    if (x_char < 1 or y_line < 1) return -1;
    byte charL;
    charL = 1;
    if (n > 10) {
        charL++;
        if (n > 100) {
            charL++;
            if (n > 1000) {
                charL++;
                if (n > 10000) { charL++; }
            }
        }
    }

    if (padTo > 0) {
        display.fillRect((x_char - 1) * textSize * charWidth,
                         (y_line - 1) * textSize * charHight,
                         (padTo + 1) * textSize * charWidth,
                         1 * textSize * charHight,
                         SH110X_WHITE);
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

void setupOLed()
{
    display.begin(i2c_Address, true); // Address 0x3C default
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
    display.println("Flow tank:0 filter:000");
    display.println("Flow, tank:0 filter:0");
    display.println("Flow, tank:0 filter:0");
    // display.setTextSize(3);
    // display.setTextColor(SH110X_BLACK, SH110X_WHITE);  // 'inverted' text
    // display.println(3.141592);
    // display.setTextSize(2);
    // display.setTextColor(SH110X_WHITE);
    // display.print("0x");
    // display.println(0xDEADBEEF, HEX);
    display.display();
    delay(2000);
}
void OLedPSIError()
{

    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(0, 0);
    display.println(F("PSI Error"));
    display.setTextSize(1);
    display.println(F("Sensor disconnected"));
    display.display();
}
