# <p align="center">XMouseFollowFocusedWindow </p>

### <p align="center">Welcome to my increasingly complex program for the X11 window system designed to teleport your cursor to the currently active window when invoked.</p>

https://github.com/Vasil-Todorov/XMouseFollowFocusedWindow/assets/154470686/5583b9fd-5f2b-4abf-ac3e-81db3530877d

The goal is to bind XMFFW with your shortcuts in some way, either directly or indirectly, to include seamlessly into your workflow. 

XMFFW can work well as a standalone action, or as a part of a multi action shortcut.

***
#### Configuration
Right now the configuration is expected to be at `/etc/XMouseFollowFocusedWindow/config.ini`
It will use the default values if you don't create one.
<details>
<summary>Example Configuration File</summary>
    
```ini
[settings]
ISSMOOTH=1
SMOOTHINCREMENT=5
SLEEPTIME=200
WARPOUTSIDE=0
ALWAYSWARP=0
DISTANCETOLERANCE=50
WARPYPLACE=90.0
WARPXPLACE=90.0
KEEP_RELATIVE_POSITION=0
```
</details>


#### "But C is hard! And my hard drive is slow! Why do I have to modify the source code or create a config file in order to customise the program?" you say?

1. I am lazy and have not implemented parameters yet

***

### "How do I install XMFFW?"

In the terminal insert these commands (The url shown may need to change depending on what repo you are reading this from)

```sh
git clone https://github.com/Vasil-Todorov/XMouseFollowFocusedWindow.git
```
```sh
sudo make install
```
Move the cursor away from the terminal and insert this command to see how the program works

```sh
xmffw
```

### "How do I uninstall XMFFW?"

In the terminal insert the following command

```sh
sudo make uninstall
```

If you want to remove leftover files from building the project 

```sh
make clean
```

***
## <p align=center> Debugging Guide </p>

If you encounter issues while using or developing **XMouseFollowFocusedWindow (XMFFW)**, you can utilize the debugging options provided in the Makefile to track down problems. Here are the steps and options available for debugging:

#### 1. Enable Debugging Symbols
To build the project with debugging symbols, run:

```sh
make debug
```

This will compile the project with debugging information using `CFLAGS_DEBUG`:

You can now run the program using the debug build xmffw-debug to investigate the issues

```sh
./xmffw-debug
```

If you ran `make` with `sudo` or as root, the `.env_build_cache` file might fail to write. You may need to remove it or change its permissions.

#### 2. Debugging with Time Information (Optional)

If you want to debug the timing aspects of the program, you can add additional timing information by enabling the DEBUG_TIME flag.

To build with timing information:

```sh
make DEBUG_TIME=1 debug
```

#### 3. Debugging Environment Changes

The Makefile includes an environment caching system to track changes in build flags. If your build is not reflecting changes to environment variables, the cache system might be preventing it. You can force a clean build:

```sh
make force
```

This will clean all debug files and force a rebuild from scratch.

#### 4. Cleaning Debug Files

To clean up after debugging:

```sh
make clean-debug
```

This will remove all debug object files and binaries without affecting the release files.

***

## <p align=center> "What do the variables do?" </p>

#### You are heavily advised to play around with these values and see what feels and looks best in your setup

1. ISSMOOTH is a bool that decides whether the cursor just teleports to the desired location, or "smoothly" goes there in increments. 1 is "smooth", 0 is not
    1. SMOOTHINCREMENT is an int that decides how large the step of the increment will be along the path
    2. SLEEPTIME is an int which decides how long the program will sleep between increments


2. WARPOUTSIDE is a bool that decides whether or not the cursor will be sent outside of the window. 1 is outside, 0 is inside.
    1. WARPXPLACE is the place that the cursor will teleport to, along the X axis. Is a percentage of the width of the window when warping inside, and a straight pixel value when warping outside.
    2. WARPYPLACE is the place that the cursor will teleport to, along the Y axis. Is a percentage of the height of the window when warping inside, and a straight pixel value when warping outside.
  
3. KEEP_RELATIVE_POSITION is a bool that decides whether the position values for WARPXPLACE & WARPYPLACE should change to the relative position of the cursor on the current window

If you are teleporting outside, whether or not WARPXPLACE and WARPYPLACE are positive or negative decides where to teleport. Negative means to the left and on top, respectively. Positive (including 0) means to the right and below, respectively.

4. ALWAYSWARP is a bool that decides whether to always teleport the mouse to the desired location when XMFFW is invoked. 1 means always teleport, 0 means see where the cursor is, and decide if it needs teleportation. If the cursor is inside of the window or within an acceptable distance of the window, it does not get teleported.
    1. DISTANCETOLERANCE is an int that describes the distance in pixels the inf. norm from the edge of the focused window, meaning extending from the window by the value of the variable in the X and Y directions. Keep in mind that it can be negative to reduce the area of the acceptable distance.

***

## <p align=center> Setup suggestions </p>
### KDE Plasma (version 5.x)

Using a tiling script like [Bismuth](https://github.com/Bismuth-Forge/bismuth) is recommended, but you can use the program standalone as well.

##### 1. Use XMFFW is as part of your tiling script, when window focus changes due to shortcuts, window spawning, etc, via "Window Action" shortcut

Go to: 
> **System Settings** -> **Shortcuts** -> **Custom Shortcuts** -> **Edit** (At the bottom) -> **New** -> **Window Action** -> **Command/URL**

Name the Custom Shortcut, and go to the **Trigger** tab. <br>
There, click on **Window gets focus**, and edit the **Window Simple** rule, by clicking **Normal** at the button, so the shortcut only affects Normal windows, not dialogs, etc.<br>
Go to **Action** and there type **xmffw** in the **Command/URL** space.

Now the cursor will warp to the focused window any time you switch focus via shortcut, open a new window, etc.

##### 2. Use XMFFW as a single action shortcut:

Go to:
> **System Settings** -> **Shortcuts** -> **Custom Shortcuts** -> **Edit** (At the bottom) -> **New** -> **Global Shortcut** -> **Command/URL**

Name the Custom Shortcut, and go to the **Trigger** tab. <br>
There, click on **Shortcut**, and enter the shortcut you would like to invoke XMFFW with.<br>
Go to **Action** and there type **xmffw** in the **Command/URL** space.

Now you can invoke XMFFW via your custom shortcut any time that you want.

#### !! Important for KDE Plasma users using XMFFW via the "Window Action" method !!

If you ever need to delete the program, keep in mind that that will not delete the shortcut. Delete the shortcut FIRST. Otherwise you might need to do some debugging when Plasma does not find the command and bugs, then the window announcing the problems gets focus, etc.

TTY is a good option to debug this from my experience.
***
### KDE Plasma (version 6.x)

Using a tiling script like [Kröhnkite](https://github.com/anametologin/krohnkite/) (which has been revived for Plasma 6!) is recommended, but you have to be more creative now, since window action activated shortcuts are deprecated :(

You can use XMFFW as a single action shortcut:

Go to:
> **System Settings** -> **Keyboard** -> **Shortcuts** -> **Add New** -> **Command or Script**, enter **xmffw**

and you can think of different shortcuts for **xmffw** in Plasma 6.

When I think of more creative implementations for  Plasma 6 I will update this readme.
***
### DWM

You can add the xmffw command to the appropriate shortcuts in the config.h and use it instead of a mouse warping patch, if you like some of my functionality better.

Or if you want to be really fancy, you can implement my code into your dwm.c file for the functionality you like!
***
### Everyone else

Be creative, invoke XMFFW manually or as part of other commands, think about all of the possibilities!

If you are on a tiling wm, you probably know how to implement this program into your workflow. If you do not, feel free  to PM me.

If you want to share the ideas from your setup, please make a pull request, bug report, etc. I will include them here!

I only included ideas for the use cases I have verified to work!

***
<br> </br> 
#### I just saw your repo, and your code is absolutely terrible!

Take it up with my main contributor Weightierharpy3 I have no clue what I am doing :P
