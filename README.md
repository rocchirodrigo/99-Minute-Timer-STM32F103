# Adjustable Timer

![alt text](https://i.postimg.cc/G29x33rK/timer-99-minutes.jpg)

In this project a Adjustable Timer up to 99 minutes was implemented on the STM32F103C8T6 development board using C language. Buttons work this way:
PA0 - sets state: in the initial state you can set up the timer, second state the timer starts ticking, and third state is used to pause the timer. 
After the initial setup you cannot adjust the timer anymore. 
PA1 - increments minutes by 1 minute.
PA2 - decrements minutes by 1 minute.

The second Led toggles as the timer is active.
Improvements may include debounce implementation for push-buttons (they're active by interruptions) and using a 4-digit display to also show seconds.

Parts utilized: 
STM32F103C8T6 Blue Pill development board
3x Push-Buttons
2x LEDs
19x 330-ohm resistors (2x Leds, 3x Push-Buttons, 14x for 2 7-segment displays)
A whole lot of jumpers (40~)

Authors: Licon Petenucci, Rodrigo Rocchi
