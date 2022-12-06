# fa21-final-project-two-rats
fa21-final-project-two-rats created by GitHub Classroom

## Abstract

This project is based on the game, Frogger. Seven VGA hardware cores and a frame buffer core are utilized to generate the player's frog, on-screen text, as well as various moving obstacle sprites and the game background. A keyboard's WASD are used to control the frog's movement. The main animated sprite is of the frog jumping whenever moving. Text on the screen will display the Level / Speed at which the obstacles move, as determined by a potentiometer (XADC). 

If time, a new video core will be implemented: All-Star Mode, which makes it harder for the player to see where they're going due to overlaying multiple colorful distractions on the screen.

## Organization

### Memory Initialization Files

1. __Sprites__: All sprites were generated by creating a .bmp image that is already accurate to the desired sprite size, limited to 4 colors to accommodate a 2-bit color palette. MATLAB was used to read the image and produce the encoded sprite sheet. The hardware is limited to 13-bits of address data, with one of those bits best reserved as data buffer to prevent sprite "corruption" (reading of the RAM in an undesired manner). And so, image pixel count per sprite must not exceed 4096px. Allowable pixel size gets smaller if more address bits are reserved for more unique sprites on the sprite sheet.

2. __Frame Buffer__: While sprites use 12-bit color (4-bits/R/G/B), the frame buffer saves block RAM space with a 9-bit color system (3-bits/R/G/B). MATLAB was also used to produce the RAM file. Files contain raw 9-bit binary, as opposed to the 2-bit color palette.


### Hardware Design Language

1. __`video_sys_daisy.sv`__: Daisy-chain of hardware cores for a stream of serial data, using the `chu_io_map.svh` I/O map modified to match new cores:
     - Frame Buffer
     - V6: Dancing Frog Sprite
     - V5: Turtle Sprite
     - V4: Log Sprite
     - V3: Car Sprite
     - V2: On-Screen Display Text
     - V1: Main Frog Sprite

2. __`vga_sprite_____core.sv`__: Wrapping circuit to interface the core with video system. Cores V1 & V3-V5 are nearly identical "Duplication Cores", allowing up to 20 sprites to be spawned on-screen at a time via a single core, as opposed to one core per sprite. Implemented by expanding the Register Map with a set of x0/y0-coordinate registers and a control register per sprite. Code was expanded with the aid of MATLAB.

3. __`_____src.sv`__: Core code for each sprite, where all have 2-bit control registers for changing the color of the sprite body and sprite select from the sprite sheet. Sprite data is read from `____ram_lut.sv`. Code for "Duplication Cores" also expanded with the aid of MATLAB.

### Driver

1. __`vga_core.cpp`__: The DupliCore class was added to accomodate "Duplication Cores" V1 & V3-V5. Main functions are to change the x0/y0-coordinates, colors, and sprite-select of each independent sprite.

### Application

types of game logic (car kills, ride log/turtle, turtle sinks)
