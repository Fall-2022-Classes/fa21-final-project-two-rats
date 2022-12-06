# fa21-final-project-two-rats
fa21-final-project-two-rats created by GitHub Classroom

## Abstract

This project is based on the game, Frogger. Seven VGA hardware cores and a frame buffer core are utilized to generate the player's frog, on-screen text, as well as various moving obstacle sprites and the game background. A keyboard's WASD are used to control the frog's movement. The main animated sprite is of the frog jumping whenever moving. Text on the screen will display the Level / Speed at which the obstacles move, as determined by a potentiometer (XADC). 

If time, a new video core will be implemented: All-Star Mode, which makes it harder for the player to see where they're going due to overlaying multiple colorful distractions on the screen.

## Organization

### Sprite Memory Initialization Files

Sprites were generated

### Hardware Design Language

1. __`mmio_sys_vanilla.sv`__: Create a new slot (4) for the Blink core and section four individual LEDs for the blinking program.

2. __`blink.sv`__: Accets a 16-bit input and write to four registers if the processor enables overwrite. Otherwise, declare four LED's and blink at the same frequency indefinitely.

3. __`ms_counter.sv`__: A wrapper for `mod_counter.sv` which produces a counter for 1ms and `ext_counter.sv` which maintains record of how many milliseconds have passed. Functionality is depicted in diagram below.

Using MATLAB to aid in the expansion of the code.

### Driver

### Application
