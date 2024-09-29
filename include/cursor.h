#ifndef CURSOR_H
#define CURSOR_H

#include <X11/Xlib.h>

//This function calculates the relative mouse position as a percentage of the window's dimensions and updates configuration items with these percentages.S
void setRelativePosition(int *windowWidth, int *windowHeight, int *relativeMouseX, int *relativeMouseY);

//This function will teleport the cursor to your desired location as smooth as possible on a screen baby
void warpSmoothly(int *destinationX, int *destinationY, int *mouseXCoordinate, int *mouseYCoordinate, Display *dsp);

//Teleport the cursor to the focused window, given its X,Y position, Height, Width, Mouse Position X and Y, and the Display)
void warpToFocusedWindow(int *focusedX, int *focusedY, int *focusedHeight, int *focusedWidth, int *mouseXCoordinate, int *mouseYCoordinate, Display *dsp);

#endif // CURSOR_H