#include "utils.h"

#include <X11/X.h>
#include <X11/Xlib.h>

#include "config.h"

int compute_destination_x(int focusedX, int focusedWidth)
{
	double warpXPlace = get_config_double("WARPXPLACE");
	return warpXPlace < 0 ? (focusedX + warpXPlace) : (focusedX + focusedWidth + warpXPlace);
}

int compute_destination_y(int focusedY, int focusedHeight)
{
	double warpYPlace = get_config_double("WARPYPLACE");
	return warpYPlace < 0 ? (focusedY + warpYPlace) : (focusedY + focusedHeight + warpYPlace);
}

//Returns the parent window of "window" (i.e. the ancestor of window
//that is a direct child of the root, or window itself if it is a direct child).
//If window is the root window, returns window.
//Necessary because some GTK apps do weird stuff with child windows
Window getTopLevelParent(Display *display, Window window)
{
	Window root;
	Window parent = window;
	Window *dummy_child;
	Window child_return;
	unsigned no_dummy;

	while (XQueryTree(display, parent, &root, &parent, &dummy_child, &no_dummy) && parent != root)
		if (dummy_child) XFree(dummy_child);

	child_return = (dummy_child && no_dummy > 0) ? dummy_child[0] - 1 : parent;
	XFree(dummy_child);

	return child_return;
}
