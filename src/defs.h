#ifndef DEFS_H
#define DEFS_H

#include <Arduino.h>

#define I2C_SDA A4 // I2C data pin
#define I2C_SCL A5 // I2C clock pin.

#define LCPumpOn 1
#define LCUpToPressure 2
#define LCErrorLostPrime 2
#define LCErrorExpansionTank 3
#define LCErrorSensor 4 // The sensor should have a voltage of 0.5 for 0 PSI. So if less the 0.5 volts there is a problem like disconnected sensor etc.

#define MCUVolt 5
#define analogReadMaxRes 1024 // MAX RESOLUTION for nano, 10 bits or 0..1024
#define resPerVolt 205        // 204.8
#define minValue 102          // (1024/5) x 0.5 =102
#define maxValue 922          // 205 x 4.5 = 922.5
#define maxRange 820          // This could vary a bit.
#define sensorMaxPSI 80
#define VoltToPIS 820 / sensorMaxPSI // depends on sensor. For 75psi max VoltToPIS =#include
#define pumpOnV HIGH

#define PinPSI A0    // Analog read pin for pump presser in psi.
#define PinPump1 3   // To relay to turn pump on/off with pressure sensor
#define PinPump2 4   // To relay to turn pump on/off borehole pump to fill tank. Float controlled
#define PinFloat1 9  // Connect to the low float
#define PinFloat2 10 // Connect to the high float
#define PinFlow1                                                                                                                                          \
    7                 // Connect to flow sensor switch, pump to first tank. This is for checking
                      // prime or any other problem with borehole pump.
#define PinFlow2 8    // As above but for system pressure pump after settlement tanks.
#define PinTemp 12    // 1Wire Temperature sensor to protect from frost.
#define PinHeater1 A2 // To relay to turn on heating.
#define PinHeater2 A3 // To relay to turn on heating.

#define lostPrimeLed 5 // LED to show lost prime

/**
 * @brief External variable representing the minimum PSI (pounds per square inch) value.
 *
 * This variable is used to define the minimum pressure threshold in the system.
 * It is declared as an external variable, meaning it is defined elsewhere in the program.
 */
extern byte minPSI;

/**
 * @brief Represents the maximum pressure in PSI (pounds per square inch).
 *
 * This variable is declared as an external byte, meaning it is defined elsewhere
 * in the program. It is used to store the maximum allowable pressure value.
 */
extern byte maxPSI;

#endif