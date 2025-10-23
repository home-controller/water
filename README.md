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

### Voltage on IO Pins
* Float up 0V   (This is where the reed switch in the float is open[^1], so the pulldown resistor on the controller PCB will pull the float pin to 0V)
* Float down 5V (This is where the reed switch in the float is closed, powering the float pin to 5V)
* TODO: Might be an idea to add a fuse or resistor to the 5V going to the float

I think it is better this way around as if the tank is full it will be the same as if the wire is disconnected with a bad connection etc.

#### Notes
[^1]: This can also mean there no float on this pin(maybe using only 1 float) or a connection problem, brocken wire etc.

### More details

* Tank level lines D9 & D10 go through a 22Ω and are also pulled to ground vi. 47k
  * So water at float Pin is pulled too 0V with the 47k resistor
  * Float down(no water) the float will connect the pin to 5V and override the pulldown resistor.
* There are 2 floats one for the low water level and 1 for the high.

### Float states
* If both pins are HIGH(+5V) the tank needs more water.
* If the top float pin is 0V the tank is full
  * note the bottom float will also be floating = switch is off now, so pulled low on PCB.
    * If bottom float is high while the top one is low there is a problem somewhere.
    * Go to single float mode?
      * Maybe leave the pump on for a time to fill the tank past the float, maybe 30 seconds
      * And make sure the pump doesn't turn on again for some time, maybe 40 seconds
      * Need to check how fast the tank fills and empties for better values.

1. Connect one side of the float switch to +5
2. Connect the other side to the float pin
3. In code check for both pins high to turn on pump
   1. Check for water flow and turn off pump until fixed if it's lost its prime etc.
4. Turn off pump when top float pins is low again.
5. TODO: maybe do something about just having 1 float or if 1 stops working.


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