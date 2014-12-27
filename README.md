twitch-cursed
=============
## Overview
An ncurses based interface for livestreamer.py.
Can manage favourite streams via a config file and also list right-now played games on stream.
The main purpose for this program, is to make my raspberry pi more easy to use with twitch.
As i continue to add on features to the plugins and plugins also, its functional range may extend.

## Notable dependencies: 
* libncurses5-dev (for gui),
* libjansson-dev (for twitch api parsing)
* cmake

## installation instructions
mkdir build && cd build && cmake ../ && make
