#include <X11/X.h>
#include <stdbool.h>
#include <X11/Xlib.h> /* defines common Xlib functions and structs. */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h> /*need to wait */
#include <math.h>
#include <string.h>
#include <errno.h>

#define MAX_LINE_LENGTH 1024
#define MAX_CONFIG_ITEMS 50
#define CONFIG_FILE_PATH "/etc/XMouseFollowFocusedWindow/config.ini"

typedef struct ConfigItem
{
	char key[MAX_LINE_LENGTH];
	char value[MAX_LINE_LENGTH];
} ConfigItem;

ConfigItem config_items[MAX_CONFIG_ITEMS];

void load_config(const char *filename)
{
	FILE *fp = fopen(filename, "r");
	if (!fp)
	{
		fprintf(stderr, "Error opening config file: %s\n", strerror(errno));
		return;
	}

	char buffer[MAX_LINE_LENGTH] = {0};

	while (fgets(buffer, sizeof(buffer), fp))
	{
		// Ignore comments and empty lines
		if (buffer[0] == ';' || buffer[0] == '#' || buffer[0] == '\n')
		{
			continue;
		}

		// Split the line into key and value
		char *key = strtok(buffer, "=");
		if (key)
		{
			char *value = strtok(NULL, "=");
			if (value)
			{
				// Trim any trailing newline or spaces from value
				value[strcspn(value, "\r\n")] = '\0';

				// Check if the key exists in the predefined config items
				bool key_found = false;
				for (int i = 0; i < MAX_CONFIG_ITEMS; i++)
				{
					if (strcmp(config_items[i].key, key) == 0)
					{
						// Update the value of the existing key
						strcpy(config_items[i].value, value);
						key_found = true;
						break;
					}
				}

				// If the key is not found, skip it (don't add new keys)
				if (!key_found)
				{
					fprintf(stderr, "Unrecognized key in config file: %s\n", key);
				}
			}
		}
	}

	fclose(fp);
}

void set_default_values()
{
	strcpy(config_items[0].key, "ISSMOOTH");
	strcpy(config_items[1].key, "SMOOTHINCREMENT");
	strcpy(config_items[2].key, "SLEEPTIME");
	strcpy(config_items[3].key, "WARPOUTSIDE");
	strcpy(config_items[4].key, "ALWAYSWARP");
	strcpy(config_items[5].key, "DISTANCETOLERANCE");
	strcpy(config_items[6].key, "WARPXPLACE");
	strcpy(config_items[7].key, "WARPYPLACE");

	strcpy(config_items[0].value, "1");
	strcpy(config_items[1].value, "5");
	strcpy(config_items[2].value, "200");
	strcpy(config_items[3].value, "0");
	strcpy(config_items[4].value, "0");
	strcpy(config_items[5].value, "50");
	strcpy(config_items[6].value, "90.0");
	strcpy(config_items[7].value, "90.0");
}

bool get_config_bool(const char *key)
{
	for (int i = 0; i < MAX_CONFIG_ITEMS; i++)
	{
		if (strcmp(config_items[i].key, key) == 0)
		{
			return strcmp(config_items[i].value, "1") == 0;
		}
	}
	return false; // default to false if not found
}

int get_config_int(const char *key)
{
	for (int i = 0; i < MAX_CONFIG_ITEMS; i++)
	{
		if (strcmp(config_items[i].key, key) == 0)
		{
			return atoi(config_items[i].value);
		}
	}
	return 0; // default to 0 if not found
}

double get_config_double(const char *key)
{
	for (int i = 0; i < MAX_CONFIG_ITEMS; i++)
	{
		if (strcmp(config_items[i].key, key) == 0)
		{
			return atof(config_items[i].value);
		}
	}
	return 0.0; // default to 0.0 if not found
}

//Returns the parent window of "window" (i.e. the ancestor of window
//that is a direct child of the root, or window itself if it is a direct child).
//If window is the root window, returns window.
Window getTopLevelParent(Display *display, Window window)
{
	//Variables to hold the current parent, root, pointer to children, etc.
	Window parent;
	Window root;
	Window *children;
	unsigned int num_children;

	//Until we have to exit because we have found the top level window, search further
	while (1)
	{
		//Query the tree of the current iteration of "window" and check if XQueryTree is "0". If true, there has been an error.
		if (0 == XQueryTree(display, window, &root, &parent, &children, &num_children))
		{
			fprintf(stderr, "XQueryTree error\n");
		}

		//Test for NULL with if
		if (children)
		{
			XFree(children);
		}

		//Check if the current iteration is either the root window or its parent is the root window.
		//If either is true, then return the window, otherwise iterate further with the parent window as the new window to look into.
		if (window == root || parent == root)
		{
			return window;
		}
		else
		{
			window = parent;
		}
	}
}

//This function will teleport the cursor to your desired location as smooth as possible on a screen baby
void warpSmoothly(int *destinationX, int *destinationY, int *mouseXCoordinate, int *mouseYCoordinate, Display *dsp)
{
	//Use doubles, as you need the precision
	//Create ints to know in which direction we are sending the cursors and fill them
	int leftToRight = -1;
	int topToBottom = -1;

	//If the destination is to the right and lower respectively, switch the directions from the default ones
	if (*destinationX > *mouseXCoordinate) leftToRight = 1;
	if (*destinationY > *mouseYCoordinate) topToBottom = 1;

	//Commit the lengths of the X and Y path, so we do less math
	int lengthOfXPath = (*destinationX - *mouseXCoordinate) * leftToRight;
	int lengthOfYPath = (*destinationY - *mouseYCoordinate) * topToBottom;

	//We also need the length of the path we need to take, in order to move at the right ratios along the X and Y axis
	double lengthOfPath = sqrt((pow(lengthOfXPath, 2) + pow(lengthOfYPath, 2)));

	//We will always need the ratio of PathX and PathY to the true path
	double ratioX = (double)lengthOfXPath / lengthOfPath;
	double ratioY = (double)lengthOfYPath / lengthOfPath;

	//Commit the X and Y increments to memory so we don't do too much math
	double howMuchToMoveX = (((double)get_config_int("SMOOTHINCREMENT")) * ratioX);
	double howMuchToMoveY = (((double)get_config_int("SMOOTHINCREMENT")) * ratioY);
	double howMuchToMove = (double)get_config_int("SMOOTHINCREMENT");

	//We use these variables to track the length of the way we need to go in the X and  Y directions respectively
	double trackY = howMuchToMoveY;
	double trackX = howMuchToMoveX;

	for (double trackPath = howMuchToMove; trackPath < lengthOfPath; trackPath += howMuchToMove)
	{
		//Increment the spots along the X and Y paths after waiting, and warp the pointer to the newly incremented place
		usleep(get_config_int("SLEEPTIME"));
		trackX += howMuchToMoveX;
		trackY += howMuchToMoveY;
		XWarpPointer(dsp, None, RootWindow(dsp, 0), 0, 0, 0, 0, (*mouseXCoordinate + trackX * leftToRight), (*mouseYCoordinate + trackY * topToBottom));
		XFlush(dsp);
	}

	//Send it the rest of the way there
	XWarpPointer(dsp, None, RootWindow(dsp, 0), 0, 0, 0, 0, *destinationX, *destinationY);
}

//Teleport the cursor to the focused window, given its X,Y position, Height, Width, Mouse Position X and Y, and the Display)
void warpToFocusedWindow(int *focusedX, int *focusedY, int *focusedHeight, int *focusedWidth, int *mouseXCoordinate, int *mouseYCoordinate, Display *dsp)
{
	//Declare destination values for the cursor
	int destinationX = 0;
	int destinationY = 0;

	//Check if you want to warp outside of the window, if not, warp inside with the normal procedure
	if (get_config_bool("WARPOUTSIDE") == 1)
	{
		//This is a nested if to figure out where the cursor should lie, to the left or to the right?
		if (get_config_double("WARPXPLACE") < 0)
		{
			//Top or bottom?
			if (get_config_double("WARPYPLACE") < 0)
			{
				destinationX = (*focusedX + get_config_double("WARPXPLACE"));
				destinationY = (*focusedY + get_config_double("WARPYPLACE"));
			}
			else
			{
				destinationX = (*focusedX + get_config_double("WARPXPLACE"));
				destinationY = (*focusedY + *focusedHeight + get_config_double("WARPYPLACE"));
			}
		}
		else
		{
			//Top or bottom?
			if (get_config_double("WARPYPLACE") < 0)
			{
				destinationX = (*focusedX + *focusedWidth + get_config_double("WARPXPLACE"));
				destinationY = (*focusedY + get_config_double("WARPYPLACE"));
			}
			else
			{
				destinationX = (*focusedX + *focusedWidth + get_config_double("WARPXPLACE"));
				destinationY = (*focusedY + *focusedHeight + get_config_double("WARPYPLACE"));
			}
		}
	}
	else
	{
		//Convert the location input into percentage and use it at the same time
		destinationX = *focusedX + ((*focusedWidth * get_config_double("WARPXPLACE")) / 100);
		destinationY = *focusedY + ((*focusedHeight * get_config_double("WARPYPLACE")) / 100);
	}
	//Warp with the coordinates we got, either smoothly or not
	if (get_config_bool("ISSMOOTH") == 1)
	{
		warpSmoothly(&destinationX, &destinationY, mouseXCoordinate, mouseYCoordinate, dsp);
	}
	else
	{
		XWarpPointer(dsp, None, RootWindow(dsp, 0), 0, 0, 0, 0, destinationX, destinationY);
	}
}

int main()
{
	set_default_values();
	load_config(CONFIG_FILE_PATH);

	//Initialise the pointer variable returned when opening a display and open the connection to the dsp
	Display *dsp = XOpenDisplay(NULL);
	if (!dsp) return 1; //if there is no Display, exit

	//Initialise structure to hold all of the Event output, namely the mouse coordinates
	XEvent event;

	//Query current pointer position
	XQueryPointer(dsp, RootWindow(dsp, DefaultScreen(dsp)),
		      &event.xbutton.root, &event.xbutton.window,
		      &event.xbutton.x_root, &event.xbutton.y_root,
		      &event.xbutton.x, &event.xbutton.y,
		      &event.xbutton.state);

	//Pump the mouse coordinates into more usable variables
	int *mouseXCoordinate = &event.xbutton.x;
	int *mouseYCoordinate = &event.xbutton.y;

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
	if (get_config_bool("ALWAYSWARP") == 1)
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
			warpToFocusedWindow(focusedX, focusedY, focusedHeight, focusedWidth, mouseXCoordinate, mouseYCoordinate, dsp);
		}
	}

	//You have to close the connection to the display!!!
	XCloseDisplay(dsp);
}
