# LED race

This is a car racing game to play on an indexable LED strip.

## Features

- players push the cars forward by pressing on a button, or by activating a PIN in any other way (e.g. by pedaling on a cyclette)
- a race can consist in one or more laps
- while playing the game, a song will play in the background progressively based on the position of the foremost player
- at the end of the game, the same song will play to celebrate the winning player
- cars are affected by friction, and will slow down if a player stops pressing the corresponding button
- since the LED strip is supposed to be hanged on a wall, gravity can be configured for each pixel to make it more realistic

## Setup

This project requires:
- An Arduino MEGA that handles the game logic and the LED strip
    - can as well be an Arduino UNO if the LED strip is not too big, but consider that the UNO has 2048bytes of RAM and each pixel on the LED strip uses 3 bytes of RAM
- An Arduino UNO that plays tunes and receives button presses
    - this Arduino is supposed to communicate with the MEGA via Serial2<->Serial
    - using just one Arduino is not enough, since updating the LED strip takes ~30ms and during that time all interrupts are disabled, which is bad if you are using `tune()` to play music
    - reading and debouncing button presses has also been moved to the secondary Arduino to better catch short impulses
- An AdaFruit-NeoPixel compatible LED strip
- A piezo buzzer (or a speaker) if you want to play music
- An additional light (or anything, actually), if you want something else to turn on while playing the winning music

## Credits

The original code was taken from [FabLabValsamoggia/LedRace](https://github.com/FabLabValsamoggia/LedRace), which is licensed under the MIT license.
The songs that can be played on the buzzer are taken from [robsoncouto/arduino-songs](https://github.com/robsoncouto/arduino-songs).
This project is free software and is licensed under GPLv3.
