#include <X11/X.h>
#include <X11/Xlib.h> /* defines common Xlib functions and structs. */
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "cursor.h"
#include "config.h"
#include "debug.h"
#include "utils.h"

int main()
{
	DEBUG_PRINT(("XMouseFollowFocusedWindow v1.0.0\n"));
	set_default_values();
	load_config(CONFIG_FILE_PATH);
	DEBUG_PRINT_CONFIG_VALUES();

	//Initialise the pointer variable returned when opening a display and open the connection to the dsp
	Display *dsp = XOpenDisplay(NULL);
	if (dsp == NULL) {
		fprintf(stderr, "Error: Cannot open display\n");
		exit(EXIT_FAILURE);
	}


	//Initialise structure to hold all of the Event output, namely the mouse coordinates
	XEvent event;

	//Query current pointer position
	XQueryPointer(dsp, DefaultRootWindow(dsp),
			  &event.xbutton.root, &event.xbutton.window,
			  &event.xbutton.x_root, &event.xbutton.y_root,
			  &event.xbutton.x, &event.xbutton.y,
			  &event.xbutton.state);

	//Get the current window that the mouse is on top of
	XWindowAttributes pointerWindowAttributes;
	XGetWindowAttributes(dsp, event.xbutton.window, &pointerWindowAttributes);

	//Port the size of the window
	int *windowWidth = &pointerWindowAttributes.width;
	int *windowHeight = &pointerWindowAttributes.height;

	//Get Relative position to window under mouse
	XQueryPointer(dsp, event.xbutton.window, 
			  &event.xbutton.root, &event.xbutton.window,
			  &event.xbutton.x_root, &event.xbutton.y_root,
			  &event.xbutton.x, &event.xbutton.y,
			  &event.xbutton.state);

	//Pump the mouse coordinates into more usable variables
	int *mouseXCoordinate = &event.xbutton.x_root;
	int *mouseYCoordinate = &event.xbutton.y_root;
	int *relativeMouseX = &event.xbutton.x;
	int *relativeMouseY = &event.xbutton.y;

	//Check if the user wants to keep the same relative position as the focused window
	if (get_config_bool("KEEP_RELATIVE_POSITION"))
	{
		setRelativePosition(windowWidth, windowHeight, relativeMouseX, relativeMouseY);
	}

	//Initialise structures for the focused window, and its parent.
	//Needed because GTK does some weird thing where the thing that holds the focus is a 1x1 child window of the parent
	Window focusedWindow;
	Window TopLevelParentofFocused;
	int feedback;

	//Holds the attributes of the window we actually want to query
	XWindowAttributes focusedWindowAttributes;

	//Gets the focused window, then sorts through its parents, thank you GTK, and gets the attributes of the parents
	XGetInputFocus(dsp, &focusedWindow, &feedback);
	TopLevelParentofFocused = getTopLevelParent(dsp, focusedWindow);
	XGetWindowAttributes(dsp, TopLevelParentofFocused, &focusedWindowAttributes);

	//Port the position and size of the window into more usable variables.
	int *focusedX = &focusedWindowAttributes.x;
	int *focusedY = &focusedWindowAttributes.y;
	int *focusedWidth = &focusedWindowAttributes.width;
	int *focusedHeight = &focusedWindowAttributes.height;

	//Check if user wants to always teleport, otherwise go through the position checks
	if (get_config_bool("ALWAYSWARP"))
	{
		warpToFocusedWindow(focusedX, focusedY, focusedHeight, focusedWidth, mouseXCoordinate, mouseYCoordinate, dsp);
	}
	else
	{
		//Check if the cursor is inside of the acceptable rectangle where it should not be teleported to the new focused screen
		if (*mouseXCoordinate < (*focusedX - get_config_int("DISTANCETOLERANCE"))
			|| *mouseXCoordinate > (*focusedX + *focusedWidth + get_config_int("DISTANCETOLERANCE"))
			|| *mouseYCoordinate < (*focusedY - get_config_int("DISTANCETOLERANCE"))
			|| *mouseYCoordinate > (*focusedY + *focusedHeight + get_config_int("DISTANCETOLERANCE")))
		{
			// TIME_FUNCTION(warpToFocusedWindow, focusedX, focusedY, focusedHeight, focusedWidth, mouseXCoordinate, mouseYCoordinate, dsp);
			warpToFocusedWindow(focusedX, focusedY, focusedHeight, focusedWidth, mouseXCoordinate, mouseYCoordinate, dsp);
		}
	}

	//You have to close the connection to the display!!!
	XCloseDisplay(dsp);
}