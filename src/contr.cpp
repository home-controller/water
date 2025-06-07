
#include "contr.h"
#include "defs.h"
#include "ui.h"

byte maxPSI = 35;
byte minPSI = 20;

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

void wellPumpOff()
{
    if (pumpStateOnWell == true) {
#if (pumpOnV == HIGH)
        digitalWrite(PinPump2, LOW);
#else
        digitalWrite(PinPump1, HIGH);
#endif
        pumpStateOnWell = false;
        Serial.println(F("Turned well pump off"));
        oledWriteAt((byte)0, 20, 5);
    }
}

/**
 * @brief Turns on the well pump if it is not already on and ensures a minimum delay
 *        between consecutive activations. (More details in @@details below)
 *
 * @details description
 * This function checks the elapsed time since the last activation of the pump to
 * ensure that it is not turned on more than once within a 30-second interval. If the
 * pump is already on, the function does nothing. Otherwise, it activates the pump
 * by setting the appropriate pin state based on the `pumpOnV` configuration. It also
 * updates the `PumpStateOn` flag, logs the action to the serial monitor, and updates
 * the OLED display.
 *
 * @note 1: The function uses a static variable to track the last activation time and
 *       introduces a delay of 500 milliseconds if the minimum interval condition
 *       is not met.
 * @note 2: lastTurnedOn is initialized at program start so the pump can't be turned on
 *       until the minimum interval has elapsed. This to prevent rapid toggling of
 *       the pump state if board is reset constantly with power supply problems etc
 *
 *
 * @details
 * @global PumpStateOn A boolean flag indicating whether the pump is currently on.
 * @global pumpOnV A macro defining the logic level to activate the pump (HIGH or LOW).
 * @global PinPump1 The pin number for controlling the pump in one configuration.
 * @global PinPump2 The pin number for controlling the pump in another configuration.
 *
 * @dependencies
 * - millis(): Used to get the current time in milliseconds since the program started.
 * - delay(): Introduces a delay in milliseconds.
 * - digitalWrite(): Sets the state of a digital pin.
 * - Serial.println(): Logs messages to the serial monitor.
 * - oledWriteAt(): Updates the OLED display at a specific position.
 */
boolean wellPumpOn()
{
    // static unsigned long lastTurnedOn = millis();
    static unsigned long lastTurnedOn =
      0; // Initialize to 0 is the same as millis() at startup. But saves if it has been initalized each time the function is called.
    if (millis() - lastTurnedOn < minTOnTime) {
        Serial.println(F("wellPumpOn: minTOnTime not elapsed"));
        delay(500);
        return false;
    }
    if (pumpStateOnWell != true) {
#if (pumpOnV == HIGH)
        digitalWrite(PinPump2, HIGH);
#else
        digitalWrite(PinPump1, LOW);
#endif
        pumpStateOnWell = true;
        Serial.println(F("Turned well pump on"));
        oledWriteAt((byte)1, 20, 5);
    }
    return true;
}

/**
 * @brief Turns on the system pump if certain conditions are met.
 *
 * @details
 * This function checks if the minimum required off time for the pump
 * has elapsed before turning it on. If the pump is already on, it
 * does nothing. When the pump is turned on, it updates the pump state,
 * logs the action to the serial monitor, and updates the OLED display.
 *
 * @return boolean
 *         - true: If the pump is successfully turned on or is already on.
 *         - false: If the pump cannot be turned on due to the minimum off time constraint.
 */
boolean systemPumpOn()
{
    static unsigned long lastTurnedOn = 0; // Initialize to 0 is the same as millis() at startup. But saves checking if initalized each time the function is called.
    if (millis() - lastTurnedOn < minTOnTime) {
        delay(500);
        return false;
    }
    if (pumpStateOnSys != true) {
#if (pumpOnV == HIGH)
        digitalWrite(PinPump1, HIGH);
#else
        digitalWrite(PinPump1, LOW);
#endif
        pumpStateOnSys = true;
        Serial.println(F("Turned sys pump on"));
        oledWriteAt((byte)1, 19, 6);
    }
    return true;
}

/**
 * @brief Turns off the system pump if it is currently on.
 *
 * @details
 * This function checks if the system pump is currently on. If it is, the function
 * turns it off by setting the appropriate pin state based on the `pumpOnV` configuration.
 * It also updates the `PumpStateOn` flag, logs the action to the serial monitor, and
 * updates the OLED display.
 *
 * @note The function does not check for any minimum off time before turning off the pump.
 *
 * @par Dependencies:
 * - digitalWrite(): Sets the state of a digital pin.
 * - Serial.println(): Logs messages to the serial monitor.
 * - oledWriteAt(): Updates the OLED display at a specific position.
 */
void systemPumpOff()
{
    if (pumpStateOnSys == true) {
#if (pumpOnV == HIGH)
        digitalWrite(PinPump1, LOW);
#else
        digitalWrite(PinPump1, HIGH);
#endif
        pumpStateOnSys = false;
        Serial.println(F("Turned sys pump off"));
        oledWriteAt((byte)0, 19, 6);
    }
}

/**
 * @brief Checks the status of the water tank using float sensors and controls the well pump accordingly.
 *
 * This function reads the states of two float sensors (top and lower) to determine the water level in the tank.
 * Based on the sensor readings, it decides whether to turn the well pump on or off. It also handles cases where
 * the float sensors might be stuck or malfunctioning.
 *
 * @return boolean
 *         - true: Indicates the tank is full.
 *         - false: Indicates the tank is not full.
 *
 * @note
 * - The function assumes the following:
 *   - `PinFloatTop` and `PinFloatLower` are defined and represent the pins connected to the float sensors.
 *   - `WaterAtFloat` and `WaterNotAtFloat` are defined constants representing the states of the float sensors.
 * - If the top float sensor (`f1`) is stuck or the lower float sensor (`f2`) is not working, the function logs
 *   a message to the serial monitor and turns off the pump.
 *
 * @todo
 * - Implement a mechanism to prevent rapid cycling of the pump.
 * - Update the UI to indicate when a float sensor is stuck.
 *
 * @test: that the float sensors are working correctly.
 * (//|#|<!--|;|/\*|^|^[ \t]*(-|\d+.))\s*($TAGS)
 */
boolean testTankFull()
{
    // should we also add a #define for the option to have only one float? Or we could do this by jumping float 2 to +5V
    byte f1, f2;
    // TEST
    f1 = digitalRead(PinFloatTop);
    f2 = digitalRead(PinFloatLower);
    if ((f1 == WaterAtFloat) and (f2 == WaterNotAtFloat)) {
        wellPumpOff(); // TODO Change the pump on and off functions to make sure they do not cycle the pump on and off too fast.
        Serial.println(F("Float 1 is stuck or float 2 is not working"));
        // TODO: Update the UI to show that the float is stuck.
        return true; // We do not really know if the tank is full or not, as the float is not working. But safer to assume it is full.
    }
    if (f1 == WaterAtFloat) {
        wellPumpOff();
        return true;
    } else if ((f1 == WaterNotAtFloat) and (f2 == WaterNotAtFloat)) {
        wellPumpOn();
        return false;
    }
    return false; // water between the 2 floats
}
