# README

i don't have a clue as to what im doing

this is just my journey into learning opengl, enjoy


~~currently it can do:~~


ok that list has since expanded to be too much to list out so INSTEAD heres what it cant to:
- no menus or loading screen
- no weapon/team selection hud
- minimap
- territory control
- 0.76
- uhh yeah a bunch of other things i cant remember

# building

i think you'll need something around C++14, but i may have accidentally used a C++17 stl feature here and there.

## WINDOWS:
The folder `ext` should be where all the libs go, refer to the CMakeLists.txt for detail
My ext tree looks something like this:
```
├───enet
│   ├───include
│   └───lib
├───fmt (you can just put the entire github repo in here)
├───freetype
│   ├───include
│   └───lib
├───glm (same here)
├───openal
|   ├───bin
│   ├───include
│   └───libs
├───sdl2
│   ├───bin
│   ├───include
│   └───lib
└───zlib
    ├───bin
    ├───include
    └───lib
```
then run generate.bat and open the solution in `build/`

# running

on windows i have to copy the DLLs from all the `ext/` modules into the executable folder.  
make sure you have all of the AoS resources there too + the shaders folder from the source tree.  
you'll also need to get Fixedsys Excelsior and put it into `font/`, named `fixedsys.ttf`


# License
I hope I did this right.
```
Copyright (C) 10se1ucgo 2018
based on pyspades <https://github.com/infogulch/pyspades> (C) Mathias Kaerlev 2011-2012.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
```
