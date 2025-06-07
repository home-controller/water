#ifndef DEFS_H
#define DEFS_H

#include <Arduino.h>

#define I2C_SDA A4 // I2C data pin
#define I2C_SCL A5 // I2C clock pin.

/**
 * @brief Maximum pressure in PSI (Pounds per Square Inch) allowed in the system.
 *
 * This variable defines the upper limit for pressure in the system to ensure
 * safe operation. It is used as a threshold to monitor and control pressure levels.
 *
 * @note The value is set to 35 PSI by default.
 * @note this variable is defined in contr.h
 */
extern byte maxPSI;// = 35
extern byte minPSI;// = 20
#define sensorMaxPSI 80 /// this needs to be set to the maximum pressure of the sensor

// For the presser sensor I am using Input 5V Output 0.5-4.5V / 0-5V(0-100PSI)
#define MCUVolt 5
#define analogReadMaxRes 1024                                  // MAX RESOLUTION for nano, 10 bits or 0..1024
constexpr float resPerVolt(analogReadMaxRes / (float)MCUVolt); // 1024/5 = 204.8
constexpr byte minValue = static_cast<byte>(resPerVolt * 0.5); // (1024/5) x 0.5 =102 this is the ADC value for 0.5V
constexpr word maxValue = static_cast<word>(resPerVolt * 4.5); // 205 x 4.5 = 922.5
#define maxRange (maxValue - minValue)                         // = 820, This could vary a bit.
#define VoltToPIS maxRange / sensorMaxPSI                      // depends on sensor. For 75psi max VoltToPIS =#include
#define pumpOnV HIGH

/**
 * @brief Minimum time interval between turning on the pump again.
 *
 * This constant defines the minimum delay (in milliseconds) required
 * before the pump can be turned on again. It is designed to protect
 * the pump from damage caused by rapid cycling, which can occur if
 * the expansion vessel lacks sufficient air. Rapid cycling can lead
 * to the pump turning on and off repeatedly, potentially causing it
 * to burn out. The default value is set to 30 seconds (30,000 milliseconds).
 */
constexpr unsigned long minTOnTime = (15UL * 1000);
//TODO: Change this to 30 seconds when done testing

#define PinPSI A0        // Analog read pin for pump presser in psi.
#define PinPump1 3       // To relay to turn pump on/off with pressure sensor
#define PinPump2 4       // To relay to turn pump on/off borehole pump to fill tank. Float controlled
#define PinFloatTop 9    // Connect to the low float
#define PinFloatLower 10 // Connect to the high float
#define WaterAtFloat 0   // The water level at the float switch. When the water is at the float then the switch in the float is off
#define WaterNotAtFloat                                                                                                                                   \
    1 // When the water level is lower than the float the switch in the float is on and as the other side is +5V the pin is HIGH
      // turn on the pump.
#define PinFlow1                                                                                                                                          \
    7                 // Connect to flow sensor switch, pump to first tank. This is for checking
                      // prime or any other problem with borehole pump.
#define PinFlow2 8    // As above but for system pressure pump after settlement tanks.
#define PinTemp 12    // 1Wire Temperature sensor to protect from frost.
#define PinHeater1 A2 // To relay to turn on heating.
#define PinHeater2 A3 // To relay to turn on heating.

#define lostPrimeLed 5 // LED to show lost prime

#define LCPumpOn 1
#define LCUpToPressure 2
#define LCErrorLostPrime 2
#define LCErrorExpansionTank 3
#define LCErrorSensor 4 // The sensor should have a voltage of 0.5 for 0 PSI. So if less the 0.5 volts there is a problem like disconnected sensor etc.

#endif