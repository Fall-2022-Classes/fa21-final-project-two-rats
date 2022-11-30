# fa21-final-project-two-rats
fa21-final-project-two-rats created by GitHub Classroom

## Abstract

This project will be based on the game, Frogger. All eight available video/sprite cores shall be utilized for the player's frog, as well as various moving obstacle sprites and the game background. A keyboard's arrow keys will be used to control the frog's movement. The main animated sprite will be of the frog jumping. Text on the screen will display the Level / Speed at which the obstacles move, as determined by the potentiometer (XADC). If time, a new video core will be implemented: All-Star Mode, which makes it harder for the player to see where they're going due to overlaying multiple colorful distractions on the screen.

OBJECT ORIENTED PROGRAMMING

## Frame Buffer Background RAM Memory Initialization File Generation Procedure

1. Convert image to 2-bit color: http://pixelatorapp.com/
2. Split image into multiple binary images using an image editor.
3. Convert each split image into binary bitmap (CUSTOM CHARACTERS "0 " AND "1 "): https://www.dcode.fr/binary-image
4. Use Notepad++ to replace binary with 2-bit color code bitmap.
5. Convert colors to RGB values: https://redketchup.io/color-picker
6. Map the 0-255 RGB values to 0-7: https://www.arduino.cc/reference/en/language/functions/math/map/ & https://www.onlinegdb.com/online_c++_compiler
7. Use Notepad++ to replace 2-bit color codes with 9-bit binary (RRR_GGG_BBB\n). Number of lines = 640 x 480 = 307,200 pixels.
