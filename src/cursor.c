#include "cursor.h"
#include <unistd.h> /*need to wait */
#include <math.h>

#include "config.h"
#include "utils.h"
#include "debug.h"

static inline void setRelativePosition_internal(int *windowWidth, int *windowHeight, int *relativeMouseX, int *relativeMouseY);
static inline void warpSmoothly_internal(int *destinationX, int *destinationY, int *mouseXCoordinate, int *mouseYCoordinate, Display *dsp);
static inline void warpToFocusedWindow_internal(int *focusedX, int *focusedY, int *focusedHeight, int *focusedWidth, int *mouseXCoordinate, int *mouseYCoordinate, Display *dsp);

void setRelativePosition(int *windowWidth, int *windowHeight, int *relativeMouseX, int *relativeMouseY) { TIME_FUNCTION_VOID(setRelativePosition_internal, windowWidth, windowHeight, relativeMouseX, relativeMouseY); }
void warpSmoothly(int *destinationX, int *destinationY, int *mouseXCoordinate, int *mouseYCoordinate, Display *dsp) { TIME_FUNCTION_VOID(warpSmoothly_internal, destinationX, destinationY, mouseXCoordinate, mouseYCoordinate, dsp); }
void warpToFocusedWindow(int *focusedX, int *focusedY, int *focusedHeight, int *focusedWidth, int *mouseXCoordinate, int *mouseYCoordinate, Display *dsp) { TIME_FUNCTION_VOID(warpToFocusedWindow_internal, focusedX, focusedY, focusedHeight, focusedWidth, mouseXCoordinate, mouseYCoordinate, dsp); }

static inline void setRelativePosition_internal(int *windowWidth, int *windowHeight, int *relativeMouseX, int *relativeMouseY)
{
	//Calculate the percentage and then modify the WARPXPLACE and WARPYPLACE variables to the new percentage
	set_config_double("WARPXPLACE", (double)*relativeMouseX / (double)*windowWidth * 100.0);
	set_config_double("WARPYPLACE", (double)*relativeMouseY / (double)*windowHeight * 100.0);
}

static inline void warpSmoothly_internal(int *destinationX, int *destinationY, int *mouseXCoordinate, int *mouseYCoordinate, Display *dsp)
{
	//Use doubles, as you need the precision
	//Create ints to know in which direction we are sending the cursors and fill them
	int leftToRight = -1;
	int topToBottom = -1;

	//If the destination is to the right and lower respectively, switch the directions from the default ones
	if (*destinationX > *mouseXCoordinate) leftToRight = 1;
	if (*destinationY > *mouseYCoordinate) topToBottom = 1;

	//Commit the lengths of the X and Y path, so we do less math
	const int lengthOfXPath = (*destinationX - *mouseXCoordinate) * leftToRight;
	const int lengthOfYPath = (*destinationY - *mouseYCoordinate) * topToBottom;

	//We also need the length of the path we need to take, in order to move at the right ratios along the X and Y axis
	const double lengthOfPath = sqrt((pow(lengthOfXPath, 2) + pow(lengthOfYPath, 2)));

	//We will always need the ratio of PathX and PathY to the true path
	const double ratioX = (double)lengthOfXPath / lengthOfPath;
	const double ratioY = (double)lengthOfYPath / lengthOfPath;

	//Commit the X and Y increments to memory so we don't do too much math
	const double howMuchToMoveX = (((double)get_config_int("SMOOTHINCREMENT")) * ratioX);
	const double howMuchToMoveY = (((double)get_config_int("SMOOTHINCREMENT")) * ratioY);
	const double howMuchToMove = (double)get_config_int("SMOOTHINCREMENT");
	const int sleeptime = usleep(get_config_int("SLEEPTIME"));

	//We use these variables to track the length of the way we need to go in the X and  Y directions respectively
	double trackY = howMuchToMoveY;
	double trackX = howMuchToMoveX;

	for (double trackPath = howMuchToMove; trackPath < lengthOfPath; trackPath += howMuchToMove)
	{
		//Increment the spots along the X and Y paths after waiting, and warp the pointer to the newly incremented place
		XWarpPointer(dsp, None, RootWindow(dsp, 0), 0, 0, 0, 0, (*mouseXCoordinate + trackX * leftToRight), (*mouseYCoordinate + trackY * topToBottom));
		trackX += howMuchToMoveX;
		trackY += howMuchToMoveY;
		XFlush(dsp);
		usleep(sleeptime);
	}

	//Send it the rest of the way there
	XWarpPointer(dsp, None, RootWindow(dsp, 0), 0, 0, 0, 0, *destinationX, *destinationY);
}

static inline void warpToFocusedWindow_internal(int *focusedX, int *focusedY, int *focusedHeight, int *focusedWidth, int *mouseXCoordinate, int *mouseYCoordinate, Display *dsp)
{
	//Declare destination values for the cursor
	int destinationX = 0;
	int destinationY = 0;

	//Check if you want to warp outside of the window, if not, warp inside with the normal procedure
	if (get_config_bool("WARPOUTSIDE") == 1)
	{
		destinationX = compute_destination_x(*focusedX, *focusedWidth);
		destinationY = compute_destination_y(*focusedY, *focusedHeight);
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