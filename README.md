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
* D8, Sensor Chack. Conected to A0(the psi sensor PIN) throught a 47k resistor, untested pull pull low to check if sensor output stays valid or goes to 0V if bad connection etc.
* D9, Float top > GRD 47k 
* D10, Float Lower  > GRD 47k
* A0(Sensor pin), D7:  > GRD 1M

* Prime lost LED D4


## Todo: Water Filtering
Consider integrating a water filtering monitoring mechanism into the system to ensure water quality. This will help remove impurities and improve the overall water condition.

- [x] Read pump pressure.
- [x] Minimum turn on time.
- [ ] LCD or some other display to show setting and state.
- [ ] Add some optional buttons to set values
- [ ] Add some sort of menu to set the turn on off pressure.
- [ ] Add temperature setup to the menu.
- [ ] Add a report problems function: cell phone text or maybe LoRa network etc.