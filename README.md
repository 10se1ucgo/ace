# README

i don't have a clue as to what im doing

this is just my journey into learning opengl, enjoy


~~currently it can do:~~

ok that list has since expanded to be too much to list out so INSTEAD heres what it cant to:
- Territory Control game mode
- Protocol version 0.76
- Some other things I don't remember

# BUILDING

You'll need a compiler supporting *at least* C++14.

## WINDOWS:
The folder `ext` should be where all the libs go, refer to the CMakeLists.txt for details
My ext tree looks something like this:
```
├───alure
│   ├───dlls
│   ├───include
│   └───lib
├───enet
│   ├───include
│   └───lib
├───fmt (you can just put the entire github repo in here)
├───freetype
│   ├───include
│   └───lib
├───glm (same here)
├───libcurl
│   ├───bin
│   ├───include
│   └───lib
├───nlohmann
│   └───nlohmann
│      └───json.hpp
├───openal
│   ├───bin
│   ├───include
│   ├───libs
├───sdl2
│   ├───bin
│   ├───include
│   └───lib
└───zlib
    ├───include
    └───lib
```

Then run generate.bat and open the solution in `build/`

# RUNNING

On Windows you'll likely have to copy the DLLs from all the `ext/` modules into the executable folder.

Make sure it also has all of the AoS resources + the shaders folder from the source tree.

You'll also need the following fonts in the `font/` folder:
- `fixedsys.ttf` (Fixedsys Excelsior)
- `AldotheApache.ttf` (Aldo the Apache)

Obviously you can use any font you'd like but make sure they are named properly.


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
