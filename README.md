# Water Control System

## Overview
This project utilizes an MCU (Arduino Nano, in my case) to control the borehole pump and implement additional functionalities such as tank level monitoring and potentially water filter monitoring.

## Pressure Sensor Setup
To regulate the pump's on/off states, use a 3-wire pressure sensor powered by 5V. The sensor output voltage ranges from 0.5V to 4.5V, depending on the pressure. 
Some example sensor options I have used include:
- 0 to 75 psi
- 0 to 100 psi
- 0 to 145 psi

## Preventing Pump Issues
To ensure the longevity of the pump and prevent overheating, consider the following precautions:

### Minimum Time Delay
If the water pressure changes too quickly between on and off due to insufficient gas in the expansion tank, it can cause overheating. 
To mitigate this, impose a minimum time delay between turning the pump on, off, and then on again.

### Todo: Prime Loss Detection
It's advisable to incorporate a mechanism to detect if the pump has lost its prime and is running dry. Running the pump without water can also lead to overheating and damage.
Consider using 1-Wire temperature sensors, amp sensors, and/or water flow sensors to assist with this.

## Todo: Tank Level Monitoring
Implement a separate system to monitor the tank level. This will allow you to keep track of the water level and take appropriate action as needed.
Tank level lines D9 & D10 are pulled to ground vi. 47k

## Pins:
### Some pins goe too SRV05-4-P-T7 , ESD and Surge Protection (TVS/ESD) chips
* D2 Jumper 5
* D3 Output for Second pump, goes to a transistor(2N3904: npn 200mA, 40V, 200mW), can be jumpered too second relay after transistor 2 or take to a separate relay module
* D4 Pump on
* D5 Lost prime red led
* D6 homeNet out & J5 & J14
* D7 J5, J14, LineIn for homeNetwork test 1M resistor and Zener to ground on line side, also 1M to ground
* D8, Sensor Check. Connected to A0(the psi sensor PIN) through a 47k resistor, untested pull pull low to check if sensor output stays valid or goes to 0V if bad connection etc.
* D9, Float top > GRD 47k (SRV05, TVS/ESD)
* D10, Float Lower  > GRD 47k (SRV05, TVS/ESD)
* D11, Flow sensor
* D12, 1Wire (SRV05, TVS/ESD)
* D13, LED (SRV05, TVS/ESD)
* A0(Sensor pin), D7:  > GRD 1M (SRV05, TVS/ESD)
* A1 Unused (SRV05, TVS/ESD)
* A2 Heater??
* A3 Heater??
* A4 STEMMA QT connectors I2C Data (SDA)
* A5 Stemma QT connectors I2C Clock(SCL)

* PE0 to PE3 are not connected.

## Todo: Water Filtering
Consider integrating a water filtering monitoring mechanism into the system to ensure water quality. This will help remove impurities and improve the overall water condition.

- [x] Read pump pressure.
- [x] Minimum turn on time.
- [x] LCD or some other display to show setting and state.
- [ ] Add some optional buttons to set values
- [ ] Add some sort of menu to set the turn on off pressure.
- [ ] Add temperature setup to the menu.
- [ ] Add a report problems function: cell phone text or maybe LoRa network etc.