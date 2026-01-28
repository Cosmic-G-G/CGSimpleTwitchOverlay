## Description
A "lightweight" twitch overlay so that you can read your chat without turning to look at your second monitor. For Windows only. 

## Features
- Transparent and click-through window
- Always at the top except for reserved windows classes
- Movable anywhere on the screen
- Edittable font

## Quickstart
After building/grabbing the executable in the Release branch, navigate to the executable's directory and run the program. Specify the Twitch username as an argument. 

`powershell`
```
cd \path\to\root\
.\TwitchApp.exe username
```
*Tip: you can create a shortcut {"C:\\path\\to\\exec.exe" yourusername} for easy access*

## Changing your font
Grab a free .ttf and place it in the same directory as the executable. Rename it to FiraCode-Regular.ttf lol

## Implementation
SFML manages window, text and event polling. This is useful for animations or extended features in the future. An asyncronous boost::asio call is used to grab IRC messages from Twitch. Can upgrade to Boost\beast if only Websockets in the future. IRC messages are parsed and drawn to the screen with SFML. 

Since this was _intended_ to be lightweight, it operates with a single thread. The logic was that the window need not be drawn every frame, only when an IRC message is recieved, hence a single thread is more appropriate than two threads, one of which is constantly drawing. 

## Issues
- Uses 8% CPU on i5 10th gen (a bit too high to call "lightweight" imo)
-


.
.
.
Had other stuff to say but i forgot and i don't wanna do this rn lol maybe ill come back here in like 5 years