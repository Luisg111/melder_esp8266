# ESP8266 pager project
This project connects the pager issued by my volunteer fire department to my "smart home" system.

It relays instructions comming from a serial connection (usb) to a remote controlled ceiling fan with light as well as a radio controlled socket outlet that controls a small lamp in our corridor.

The charging station of my pager comes with a 5-Port DIN connector ([as shown in the manual](https://www.funkhandel.com/mediafiles/Sonstiges/Bedienungsanleitung/bda-lgra.pdf)) that can be used to control external appliances.
When the pager receives an alarm signal, a relay will connect the pins 1 and 3 together. Usually this is used to switch on lamps other small appliances.

The microcontroller can read the state of the pager by applying a voltage to one of the pins using a pullup resistor. If the voltage can be read on the other pin, the relay is closed and an alarm has been received.
If an alarm occurs, the microcontroller will send a instruction over the serial connection to notify the server about the incoming alarm. It will also turn on the lamp in the ceiling fan and the small lamp connected to the radio controlled socket outlet. After 5 minutes, the lights will be switched off again automatically.
