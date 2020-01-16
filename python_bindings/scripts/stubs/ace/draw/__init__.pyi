import ace
from typing import *
from typing import Iterable as iterable, Iterator as iterator
import ace.gl
import ace.glm
__all__  = [
"DebugDraw",
"VXLBlocks"
]
class DebugDraw():
    def draw_cube(self, arg0: ace.glm.vec3, arg1: ace.glm.vec3, arg2: ace.glm.vec3) -> None: 
        """draw_cube(self: ace.draw.DebugDraw, arg0: ace.glm.vec3, arg1: ace.glm.vec3, arg2: ace.glm.vec3) -> None
"""

    def draw_line(self, arg0: ace.glm.vec3, arg1: ace.glm.vec3, arg2: ace.glm.vec3) -> None: 
        """draw_line(self: ace.draw.DebugDraw, arg0: ace.glm.vec3, arg1: ace.glm.vec3, arg2: ace.glm.vec3) -> None
"""

    def draw_quad(self, arg0: ace.glm.vec3, arg1: ace.glm.vec3, arg2: ace.glm.vec3, arg3: ace.glm.vec3, arg4: ace.glm.vec3) -> None: 
        """draw_quad(self: ace.draw.DebugDraw, arg0: ace.glm.vec3, arg1: ace.glm.vec3, arg2: ace.glm.vec3, arg3: ace.glm.vec3, arg4: ace.glm.vec3) -> None
"""

    def draw_ray(self, arg0: ace.glm.vec3, arg1: ace.glm.vec3, arg2: ace.glm.vec3) -> None: 
        """draw_ray(self: ace.draw.DebugDraw, arg0: ace.glm.vec3, arg1: ace.glm.vec3, arg2: ace.glm.vec3) -> None
"""

    def draw_triangle(self, arg0: ace.glm.vec3, arg1: ace.glm.vec3, arg2: ace.glm.vec3, arg3: ace.glm.vec3) -> None: 
        """draw_triangle(self: ace.draw.DebugDraw, arg0: ace.glm.vec3, arg1: ace.glm.vec3, arg2: ace.glm.vec3, arg3: ace.glm.vec3) -> None
"""

    pass

class VXLBlocks():
    @overload
    def __init__(self, arg0: ace.glm.u8vec3) -> None: 
        """__init__(*args, **kwargs)
Overloaded function.

1. __init__(self: ace.draw.VXLBlocks, arg0: ace.glm.u8vec3) -> None

2. __init__(self: ace.draw.VXLBlocks, arg0: Dict[ace.glm.ivec3, ace.glm.u8vec3], arg1: ace.glm.vec3) -> None
"""

    @overload
    def __init__(self, arg0: Dict[ace.glm.ivec3, ace.glm.u8vec3], arg1: ace.glm.vec3) -> None: ...

    def draw(self, arg0: ace.gl.ShaderProgram) -> None: 
        """draw(self: ace.draw.VXLBlocks, arg0: ace.gl.ShaderProgram) -> None
"""

    def update(self, arg0: Dict[ace.glm.ivec3, ace.glm.u8vec3], arg1: ace.glm.vec3) -> None: 
        """update(self: ace.draw.VXLBlocks, arg0: Dict[ace.glm.ivec3, ace.glm.u8vec3], arg1: ace.glm.vec3) -> None
"""

    @property
    def centroid(self) -> ace.glm.vec3:
        """
:type: ace.glm.vec3"""
    @centroid.setter
    def centroid(self, arg0: ace.glm.vec3) -> None:
        ...

    @property
    def position(self) -> ace.glm.vec3:
        """
:type: ace.glm.vec3"""
    @position.setter
    def position(self, arg0: ace.glm.vec3) -> None:
        ...

    @property
    def rotation(self) -> ace.glm.vec3:
        """
:type: ace.glm.vec3"""
    @rotation.setter
    def rotation(self, arg0: ace.glm.vec3) -> None:
        ...

    @property
    def scale(self) -> ace.glm.vec3:
        """
:type: ace.glm.vec3"""
    @scale.setter
    def scale(self, arg0: ace.glm.vec3) -> None:
        ...

    pass
