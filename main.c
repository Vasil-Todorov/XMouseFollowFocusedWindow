#include <X11/X.h>
#include <stdbool.h>
#include <X11/Xlib.h> /* defines common Xlib functions and structs. */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h> /*need to wait */
#include <math.h>
//IMPORTANT! Keep in mind that in X, x is the left to right coordinate and it starts at the left corner
//And y is the top to bottom coordinate and starts at the top. e.g. 0,0 is the top left corner of your screens

//Modify these variables and recompile the program in order to tune the settings to your liking

//Declare if you want your cursor to moove smoothly or to just warp to the location, 1 for smooth, 0 for warp 
//ISSMOOTH makes your cursor travel the distance it should in increments
const bool ISSMOOTH=1;

//This variable will be how many pixels your cursor should cross in a leap
const int SMOOTHINCREMENT = 5;

//This variable will be the time the loop sleeps between increments, in microseconds. usleep is not the most reliable function, so play with this variable, see what works for you
const int SLEEPTIME = 200;

//Declare if you want your cursor to warp ourside of the window. If it does, values for where are treated as abosulte values in pixels.
//You can use - and + to determine whether it's top or bottom, left or right.
//For X - is left, + is right. For Y - is top, + is bottom. 0 is Warp inside, 1 is Warp outside of the window.
const bool WARPOUTSIDE=0;

//If warpOutside!=0, the pointer will be teleported inside of the window and the values for where are treated as relative
//percentage values of the size of the window.
//X goes from left to right, e.g. 0 is the left side, 100 is the right, Y goes from top to bottom, e.g. 0 is the top, 100 is the bottom
const double WARPXPLACE=90;
const double WARPYPLACE=90;

//Always teleport to the desired place, even when already inside of the window.
//1 is always, 0 is check if the window is inside and/or inside of the "tolerance zone"
const bool ALWAYSWARP=0;

//Declare how much the tolerance for distance from the window should be i.e. how far outside of the active window can the cursor be before it gets teleported to the desired place
//Treated as a rectangle outside of the window
const int DISTANCETOLERANCE=50;


//Returns the parent window of "window" (i.e. the ancestor of window
//that is a direct child of the root, or window itself if it is a direct child).
//If window is the root window, returns window.
Window getTopLevelParent(Display * display, Window window)
{
     Window parent;
     Window root;
     Window * children;
     unsigned int num_children;

     //Until we have to exit because we have found the root window, go down the tree
     while (1) {
         if (0 == XQueryTree(display, window, &root, &parent, &children, &num_children)) 
	 {
             fprintf(stderr, "XQueryTree error\n");
         }
         if (children) { //must test for null
             XFree(children);
         }
         if (window == root || parent == root) {
             return window;
         }
         else 
	 {
             window = parent;
         }
     }
}


//This function will teleport the cursor to your desired location as smooth as possible on a screen baby
void warpSmoothly(int *destinationX, int * destinationY, int * mouseXCoordinate, int * mouseYCoordinate, Display *dsp)
{
	//Use doubles, as you need the precision
	//Create ints to know in which direction we are sending the cursors and fill them
	int leftToRight = -1;
	int topToBottom = -1;

	//If the destination is to the right and lower respectively, switch the directions from the default ones
	if (*destinationX > *mouseXCoordinate ) leftToRight = 1;
	if (*destinationY > *mouseYCoordinate ) topToBottom = 1;
	
	//Commit the lengths of the X and Y path, so we do less math
	int lengthOfXPath = (*destinationX - *mouseXCoordinate)*leftToRight;
	int lengthOfYPath = (*destinationY - *mouseYCoordinate)*topToBottom;
	
	//We also need the length of the path we need to take, in order to move at the right ratios along the X and Y axis
	double lengthOfPath = sqrt((pow(lengthOfXPath, 2)+pow(lengthOfYPath, 2))); 
	
	//We will always need the ratio of PathX and PathY to the true path
	double ratioX = (double) lengthOfXPath/lengthOfPath;
	double ratioY = (double) lengthOfYPath/lengthOfPath;
  
	//Commit the X and Y increments to memory so we don't do too much math
	double howMuchToMoveX = (((double)SMOOTHINCREMENT)*ratioX);
	double howMuchToMoveY = (((double)SMOOTHINCREMENT)*ratioY);
	double howMuchToMove = (double)SMOOTHINCREMENT;

	//We use these variables to track the length of the way we need to go in the X and  Y directions respectively
	double trackY = howMuchToMoveY;
	double trackX = howMuchToMoveX;

	for( double trackPath = howMuchToMove; trackPath < lengthOfPath; trackPath += howMuchToMove)
	{
		//Increment the spots along the X and Y paths after waiting, and warp the pointer to the newly incremented place
		usleep(SLEEPTIME);
		trackX += howMuchToMoveX;
		trackY += howMuchToMoveY;
		XWarpPointer(dsp,None, RootWindow(dsp, 0), 0, 0, 0, 0, (*mouseXCoordinate + trackX*leftToRight), (*mouseYCoordinate + trackY*topToBottom));
		XFlush(dsp);

	}
	
	//Send it the rest of the way there
	XWarpPointer(dsp, None, RootWindow(dsp, 0), 0, 0, 0, 0, *destinationX, *destinationY);

}


//Teleport the cursor to the focused window, given its X,Y position, Height, Width, Mouse Position X and Y, and the Display)
void warpToFocusedWindow(int *focusedX, int *focusedY, int *focusedHeight, int  *focusedWidth, int *mouseXCoordinate, int *mouseYCoordinate, Display *dsp)
{
	//Declare destination values for the cursor
	int destinationX=0;
	int destinationY=0;
	
	//Check if you want to warp outside of the window, if not, warp inside with the normal procedure
	if(WARPOUTSIDE==1)
	{
		//This is a nested if to figure out where the cursor should lie, to the left or to the right?
		if(WARPXPLACE < 0)
		{
			//Top or bottom?
			if(WARPYPLACE < 0)
			{
				destinationX=(*focusedX+WARPXPLACE);
				destinationY=(*focusedY+WARPYPLACE);
			}
			else 
			{
				destinationX=(*focusedX+WARPXPLACE);
				destinationY=(*focusedY+*focusedHeight+WARPYPLACE);
			}
		}
		else
		{
			//Top or bottom?
			if(WARPYPLACE < 0 )
			{
				destinationX=(*focusedX+*focusedWidth+WARPXPLACE);
				destinationY=(*focusedY+WARPYPLACE);
			}
			else 
			{
				destinationX=(*focusedX+*focusedWidth+WARPXPLACE);
				destinationY=(*focusedY+*focusedHeight+WARPYPLACE);
			}
		}	
	}
	else 
	{
		//Convert the location input into percentage and use it at the same time
		destinationX=*focusedX+((*focusedWidth*WARPXPLACE)/100);
		destinationY=*focusedY+((*focusedHeight*WARPYPLACE)/100);
	}
	//Warp with the coordinates we got, either smoothly or not
	if(ISSMOOTH==1)
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

//Initialise the pointer variable returned when opening a display and open the connection to the dsp
Display *dsp = XOpenDisplay( NULL );
if( !dsp ){ return 1; } //if there is no Display, exit

//Initialise structure to hold all of the Event output, namely the mouse coordinates
XEvent event; 

//Query current pointer position
XQueryPointer(dsp, RootWindow(dsp, DefaultScreen(dsp)),
&event.xbutton.root, &event.xbutton.window,
&event.xbutton.x_root, &event.xbutton.y_root,
&event.xbutton.x, &event.xbutton.y,
&event.xbutton.state);

//Pump the mouse coordinates into more usable variables
int *mouseXCoordinate=&event.xbutton.x;
int *mouseYCoordinate=&event.xbutton.y;

//Initialise structures for the focused window, and its parent.
//Needed because GTK does some weird thing where the thing that holds the focus is a 1x1 child window of the parent
Window focusedWindow;
Window TopLevelParentofFocused;
int feedback;

//Holds the attributes of the window we actually want to query
XWindowAttributes focusedWindowAttributes;

//Gets the focused window, then sorts through its parents, thank you GTK, and gets the attributes of the parents
XGetInputFocus(dsp, &focusedWindow, &feedback);
TopLevelParentofFocused=getTopLevelParent(dsp, focusedWindow);
XGetWindowAttributes(dsp, TopLevelParentofFocused, &focusedWindowAttributes);

//Port the position and size of the window into more usable variables.
int *focusedX=&focusedWindowAttributes.x;
int *focusedY=&focusedWindowAttributes.y;
int *focusedWidth=&focusedWindowAttributes.width;
int *focusedHeight=&focusedWindowAttributes.height;

//Check if user wants to always teleport, otherwise go through the position checks
if(ALWAYSWARP==1)
{
	warpToFocusedWindow(focusedX, focusedY, focusedHeight, focusedWidth, mouseXCoordinate, mouseYCoordinate,dsp);
}
else
{
	//Check if the cursor is inside of the acceptable rectangle where it should not be teleported to the new focused screen
	if(*mouseXCoordinate < (*focusedX - DISTANCETOLERANCE) 
	|| *mouseXCoordinate > (*focusedX+*focusedWidth+DISTANCETOLERANCE) 
	|| *mouseYCoordinate < (*focusedY - DISTANCETOLERANCE) 
	|| *mouseYCoordinate > (*focusedY + *focusedHeight + DISTANCETOLERANCE) ) {
		warpToFocusedWindow(focusedX, focusedY, focusedHeight, focusedWidth, mouseXCoordinate, mouseYCoordinate,dsp);
	}
}

//You have to close the connection to the display!!!
XCloseDisplay( dsp );
}
