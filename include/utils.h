#ifndef UTILS_H
#define UTILS_H

#include <X11/Xlib.h>

int compute_destination_x(int focusedX, int focusedWidth);
int compute_destination_y(int focusedY, int focusedHeight);
Window getTopLevelParent(Display *display, Window window);

#endif