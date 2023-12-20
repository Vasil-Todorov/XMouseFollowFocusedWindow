## <div style="text-align: center"> XMouseFollowFocusedWindow </div> 
<video "src=>" **insert video**
### Welcome to my simple program for the X11 window system designed to teleport your cursor to the currently active window when invoked.

The point is to bind it with your shortcuts in some way, either directly, like in DWM, or indirectly, like invoking it every time a window gets focus in KDE Plasma, through the custom shortcuts.

There is no config file, you modify the values in the source and recompile in order to modify the functionality.

***

#### "But C is hard! Why do I have to modify the source code in order to customise the program?" You say

1. You do not have to look at the C code at all, just the top few lines in order to modify the values and then recompile with them.
2. It encourages you to look under the hood of this ***very simple*** program, in order to either make modifications yourself or understand what you are running, not just believe I am not stealing all of your data.
3. I am lazy and have not implemented a config file yet
4. Might be 0.0000001s slower with a config file lookup, think of the people with hard drives, what if they have to invoke it a few times per second.

***

### What do I do?

In the terminal insert these commands
> git clone **insert link**
> 
> sudo make install

Move the cursor away from the terminal and insert this command to see how the program works

> ./xmffw

If it is to your liking, great! If not, please open main.c with a text editor and follow the instructions in the file to modify the program to your liking.

After that you neeed to

> sudo make install

again, to update the binary that you are using

#### KDE Plasma

Using a tiling script like [Bismuth](https://github.com/Bismuth-Forge/bismuth) is recommended, but you can use the program standalone as well.

Go to: 
> **System Settings** -> **Shortcuts** -> **Custom Shortcuts** -> **Edit** (At the bottom) -> **New** -> **Window Action** -> **Command/URL**

Name the Custom Shortcut, and go to the **Trigger** tab. <br>
There, click on **Window gets focus**, and edit the **Window Simple** rule, by clicking **Normal** at the button, so the shortcut only affects Normal windows, not dialogs, etc.<br>
Go to **Action** and there type **xmffw** in the **Command/URL** space.

#### Important for KDE Plasma users

If you ever need to delete the program, keep in mind that that will not delete the shortcut. Delete the shortcut FIRST. Otherwise you might need to do some debugging when Plasma does not find the command and bugs, then the window announcing the problems gets focus, etc.

TTY is a good option to debug this from my experience.

#### DWM

You can add the xmffw command to the appropriate shortcuts in the config.h and use it instead of a mouse warping patch, if you like some of my functionality better.

Or if you want to be really fancy, you can implement my code into your dwm.c file for the functionality you like!

#### Everyone else

Be creative! If you are on a tiling wm, you probably know how to implement this program into your workflow. If you do not, feel free  to PM me.

If you want to share the ideas from your setup, please make a pull request, bug report, etc. I will include them here!

I only included ideas for the use cases I have verified to work!

***

#### "What do the variables do?"

1. ISSMOOTH is a bool that decides whether the cursor just teleports to the desired location, or "smoothly" goes there in increments. 1 is "smooth", 0 is not
    1. SMOOTHINCREMENT is an int that decides how large the step of the increment will be along the path
    2. SLEEPTIME is an int which decides how long the program will sleep between increments

You are heavily advised to play around with these values and see what fits in and looks best in your setup

2. WARPOUTSIDE is a bool that decides whether or not the cursor will be sent outside of the window.1 is outside, 0 is inside
    1. WARPXPLACE is the place that the cursor will teleport to, along the X axis. Is a percentage of the width of the window when warping inside, and a straight pixel value when warping outside.
    2. WARPYPLACE is the place that the cursor will teleport to, along the Y axis. Is a percentage of the height of the window when warping inside, and a straight pixel value when warping outside.

If you are teleporting outside, whether or not WARPXPLACE and YPLACE are positive or negative decides where to teleport. Negative means to the left and on top respectively, Positive (including 0) means to the right respectively.

3. ALWAYSWARP is a bool that decides whether to always teleport the mouse to the desired location when XMFFW is invoked. 1 means always teleport, 0 means see where the cursor is, and decide if it needs teleportation. If the cursor is inside of the window or within an acceptable distance of the window, it does not get teleported.
    1. DISTANCETOLERANCE is an int that describes the distance in the inf. norm, meaning a extending from the window by the value. Keep in mind that it can be negative to reduce the area of the acceptable distance.

***
#### I just saw your repo, and your code is absolutely terrible!

Yes, yes it is. This is my first C project, and even if it were my 10th, it would probably still be bad. If you are a C wizard or just have an idea of how to make the project better, and/or structure the code better, please send a pull request, make a bug report, etc. I am thankful for the feedback and ideas.
