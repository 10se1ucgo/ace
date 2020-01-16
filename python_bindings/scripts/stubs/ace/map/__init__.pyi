"""Python bindings to AceMap =)"""
import ace
from typing import *
from typing import Iterable as iterable, Iterator as iterator
import ace.glm
import enum
__all__  = [
"AceMap",
"Face",
"block_line",
"draw2vox",
"get_face",
"get_pos",
"is_valid_pos",
"jit_color",
"next_block",
"vox2draw",
"MAP_X",
"MAP_Y",
"MAP_Z"
]
class AceMap():
    def get_block(self, x: int, y: int, z: int, wrapped: bool=False) -> tuple: 
        """get_block(self: ace.map.AceMap, x: int, y: int, z: int, wrapped: bool=False) -> tuple
"""

    def get_color(self, x: int, y: int, z: int, wrapped: bool=False) -> ace.glm.u8vec4: 
        """get_color(self: ace.map.AceMap, x: int, y: int, z: int, wrapped: bool=False) -> ace.glm.u8vec4
"""

    def get_vis(self, x: int, y: int, z: int, wrapped: bool=False) -> int: 
        """get_vis(self: ace.map.AceMap, x: int, y: int, z: int, wrapped: bool=False) -> int
"""

    def get_z(self, x: int, y: int, start: int=0, wrapped: bool=False) -> int: 
        """get_z(self: ace.map.AceMap, x: int, y: int, start: int=0, wrapped: bool=False) -> int
"""

    def hitscan(self, arg0: ace.glm.vec3, arg1: ace.glm.vec3) -> tuple: 
        """hitscan(self: ace.map.AceMap, arg0: ace.glm.vec3, arg1: ace.glm.vec3) -> tuple
"""

    def is_solid(self, x: int, y: int, z: int, wrapped: bool=False) -> bool: 
        """is_solid(self: ace.map.AceMap, x: int, y: int, z: int, wrapped: bool=False) -> bool
"""

    def read(self, arg0: str) -> None: 
        """read(self: ace.map.AceMap, arg0: str) -> None
"""

    def set_solid(self, x: int, y: int, z: int, solid: bool, wrapped: bool=False) -> bool: 
        """set_solid(self: ace.map.AceMap, x: int, y: int, z: int, solid: bool, wrapped: bool=False) -> bool
"""

    pass

class Face(enum.IntEnum, int, enum.Enum):
    """An enumeration."""
    BACK = 2
    BOTTOM = 5
    FRONT = 3
    INVALID = -1
    LEFT = 0
    RIGHT = 1
    TOP = 4
    pass

def block_line(arg0: ace.glm.ivec3, arg1: ace.glm.ivec3) -> List[ace.glm.ivec3]:
    """block_line(arg0: ace.glm.ivec3, arg1: ace.glm.ivec3) -> List[ace.glm.ivec3]
"""
def draw2vox(arg0: ace.glm.vec3) -> ace.glm.vec3:
    """draw2vox(arg0: ace.glm.vec3) -> ace.glm.vec3
"""
def get_face(arg0: int, arg1: int, arg2: int, arg3: ace.map.Face) -> ace.glm.vec3:
    """get_face(arg0: int, arg1: int, arg2: int, arg3: ace.map.Face) -> ace.glm.vec3
"""
def get_pos(arg0: int, arg1: int, arg2: int) -> int:
    """get_pos(arg0: int, arg1: int, arg2: int) -> int
"""
@overload
def is_valid_pos(arg0: int, arg1: int, arg2: int) -> bool:
    """is_valid_pos(*args, **kwargs)
Overloaded function.

1. is_valid_pos(arg0: int, arg1: int, arg2: int) -> bool

2. is_valid_pos(arg0: int) -> bool

3. is_valid_pos(arg0: ace.glm.ivec3) -> bool
"""
@overload
def is_valid_pos(arg0: ace.glm.ivec3) -> bool:
    pass
@overload
def is_valid_pos(arg0: int) -> bool:
    pass
def jit_color(arg0: ace.glm.u8vec3) -> ace.glm.u8vec3:
    """jit_color(arg0: ace.glm.u8vec3) -> ace.glm.u8vec3
"""
def next_block(arg0: int, arg1: int, arg2: int, arg3: ace.map.Face) -> ace.glm.ivec3:
    """next_block(arg0: int, arg1: int, arg2: int, arg3: ace.map.Face) -> ace.glm.ivec3
"""
def vox2draw(arg0: ace.glm.vec3) -> ace.glm.vec3:
    """vox2draw(arg0: ace.glm.vec3) -> ace.glm.vec3
"""
MAP_X = 512
MAP_Y = 512
MAP_Z = 64
