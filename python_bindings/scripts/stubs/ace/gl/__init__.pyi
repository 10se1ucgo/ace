import ace
import ace.gl
from typing import *
from typing import Iterable as iterable, Iterator as iterator
__all__  = [
"ShaderManager",
"ShaderProgram",
"Uniforms3D",
"Blend",
"DepthMask",
"DepthTest"
]
class ShaderManager():
    @property
    def billboard(self) -> ace.gl.ShaderProgram:
        """
:type: ace.gl.ShaderProgram"""
    @property
    def line(self) -> ace.gl.ShaderProgram:
        """
:type: ace.gl.ShaderProgram"""
    @property
    def map(self) -> ace.gl.ShaderProgram:
        """
:type: ace.gl.ShaderProgram"""
    @property
    def model(self) -> ace.gl.ShaderProgram:
        """
:type: ace.gl.ShaderProgram"""
    @property
    def sprite(self) -> ace.gl.ShaderProgram:
        """
:type: ace.gl.ShaderProgram"""
    @property
    def text(self) -> ace.gl.ShaderProgram:
        """
:type: ace.gl.ShaderProgram"""
    pass

class ShaderProgram():
    def bind(self) -> None: 
        """bind(self: ace.gl.ShaderProgram) -> None
"""

    @staticmethod
    def unbind() -> None: 
        """unbind() -> None
"""

    @overload
    def uniform(self, arg0: str, arg1: int, arg2: int) -> None: 
        """uniform(*args, **kwargs)
Overloaded function.

1. uniform(self: ace.gl.ShaderProgram, name: str, mat: ace.glm.mat3, transpose: bool=False) -> None

2. uniform(self: ace.gl.ShaderProgram, name: str, mat: ace.glm.mat4, transpose: bool=False) -> None

3. uniform(self: ace.gl.ShaderProgram, arg0: str, arg1: int) -> None

4. uniform(self: ace.gl.ShaderProgram, arg0: str, arg1: int, arg2: int) -> None

5. uniform(self: ace.gl.ShaderProgram, arg0: str, arg1: int, arg2: int, arg3: int) -> None

6. uniform(self: ace.gl.ShaderProgram, arg0: str, arg1: int, arg2: int, arg3: int, arg4: int) -> None

7. uniform(self: ace.gl.ShaderProgram, arg0: str, arg1: int) -> None

8. uniform(self: ace.gl.ShaderProgram, arg0: str, arg1: int, arg2: int) -> None

9. uniform(self: ace.gl.ShaderProgram, arg0: str, arg1: int, arg2: int, arg3: int) -> None

10. uniform(self: ace.gl.ShaderProgram, arg0: str, arg1: int, arg2: int, arg3: int, arg4: int) -> None

11. uniform(self: ace.gl.ShaderProgram, arg0: str, arg1: float) -> None

12. uniform(self: ace.gl.ShaderProgram, arg0: str, arg1: float, arg2: float) -> None

13. uniform(self: ace.gl.ShaderProgram, arg0: str, arg1: float, arg2: float, arg3: float) -> None

14. uniform(self: ace.gl.ShaderProgram, arg0: str, arg1: float, arg2: float, arg3: float, arg4: float) -> None

15. uniform(self: ace.gl.ShaderProgram, arg0: str, arg1: ace.glm.ivec2) -> None

16. uniform(self: ace.gl.ShaderProgram, arg0: str, arg1: ace.glm.ivec3) -> None

17. uniform(self: ace.gl.ShaderProgram, arg0: str, arg1: ace.glm.ivec4) -> None

18. uniform(self: ace.gl.ShaderProgram, arg0: str, arg1: ace.glm.u8vec2) -> None

19. uniform(self: ace.gl.ShaderProgram, arg0: str, arg1: ace.glm.u8vec3) -> None

20. uniform(self: ace.gl.ShaderProgram, arg0: str, arg1: ace.glm.u8vec3) -> None

21. uniform(self: ace.gl.ShaderProgram, arg0: str, arg1: ace.glm.vec2) -> None

22. uniform(self: ace.gl.ShaderProgram, arg0: str, arg1: ace.glm.vec3) -> None

23. uniform(self: ace.gl.ShaderProgram, arg0: str, arg1: ace.glm.vec4) -> None
"""

    @overload
    def uniform(self, arg0: str, arg1: float, arg2: float) -> None: ...

    @overload
    def uniform(self, name: str, mat: ace.glm.mat3, transpose: bool=False) -> None: ...

    @overload
    def uniform(self, arg0: str, arg1: ace.glm.u8vec3) -> None: ...

    @overload
    def uniform(self, arg0: str, arg1: int) -> None: ...

    @overload
    def uniform(self, name: str, mat: ace.glm.mat4, transpose: bool=False) -> None: ...

    @overload
    def uniform(self, arg0: str, arg1: ace.glm.vec3) -> None: ...

    @overload
    def uniform(self, arg0: str, arg1: ace.glm.ivec4) -> None: ...

    @overload
    def uniform(self, arg0: str, arg1: float) -> None: ...

    @overload
    def uniform(self, arg0: str, arg1: ace.glm.vec2) -> None: ...

    @overload
    def uniform(self, arg0: str, arg1: int, arg2: int, arg3: int) -> None: ...

    @overload
    def uniform(self, arg0: str, arg1: float, arg2: float, arg3: float, arg4: float) -> None: ...

    @overload
    def uniform(self, arg0: str, arg1: ace.glm.ivec3) -> None: ...

    @overload
    def uniform(self, arg0: str, arg1: ace.glm.u8vec3) -> None: ...

    @overload
    def uniform(self, arg0: str, arg1: ace.glm.vec4) -> None: ...

    @overload
    def uniform(self, arg0: str, arg1: int, arg2: int, arg3: int, arg4: int) -> None: ...

    @overload
    def uniform(self, arg0: str, arg1: ace.glm.u8vec2) -> None: ...

    @overload
    def uniform(self, arg0: str, arg1: float, arg2: float, arg3: float) -> None: ...

    @overload
    def uniform(self, arg0: str, arg1: ace.glm.ivec2) -> None: ...

    pass

class Uniforms3D():
    @property
    def cam_forward(self) -> ace.glm.vec3:
        """
:type: ace.glm.vec3"""
    @cam_forward.setter
    def cam_forward(self, arg0: ace.glm.vec3) -> None:
        ...

    @property
    def cam_pos(self) -> ace.glm.vec3:
        """
:type: ace.glm.vec3"""
    @cam_pos.setter
    def cam_pos(self, arg0: ace.glm.vec3) -> None:
        ...

    @property
    def cam_right(self) -> ace.glm.vec3:
        """
:type: ace.glm.vec3"""
    @cam_right.setter
    def cam_right(self, arg0: ace.glm.vec3) -> None:
        ...

    @property
    def cam_up(self) -> ace.glm.vec3:
        """
:type: ace.glm.vec3"""
    @cam_up.setter
    def cam_up(self, arg0: ace.glm.vec3) -> None:
        ...

    @property
    def fog_color(self) -> ace.glm.vec3:
        """
:type: ace.glm.vec3"""
    @fog_color.setter
    def fog_color(self, arg0: ace.glm.vec3) -> None:
        ...

    @property
    def fog_end(self) -> float:
        """
:type: float"""
    @fog_end.setter
    def fog_end(self, arg0: float) -> None:
        ...

    @property
    def fog_start(self) -> float:
        """
:type: float"""
    @fog_start.setter
    def fog_start(self, arg0: float) -> None:
        ...

    @property
    def light_pos(self) -> ace.glm.vec3:
        """
:type: ace.glm.vec3"""
    @light_pos.setter
    def light_pos(self, arg0: ace.glm.vec3) -> None:
        ...

    @property
    def proj(self) -> ace.glm.mat4:
        """
:type: ace.glm.mat4"""
    @proj.setter
    def proj(self, arg0: ace.glm.mat4) -> None:
        ...

    @property
    def pv(self) -> ace.glm.mat4:
        """
:type: ace.glm.mat4"""
    @pv.setter
    def pv(self, arg0: ace.glm.mat4) -> None:
        ...

    @property
    def view(self) -> ace.glm.mat4:
        """
:type: ace.glm.mat4"""
    @view.setter
    def view(self, arg0: ace.glm.mat4) -> None:
        ...

    pass

def Blend(arg0: bool) -> None:
    """Blend(arg0: bool) -> None
"""
def DepthMask(arg0: bool) -> None:
    """DepthMask(arg0: bool) -> None
"""
def DepthTest(arg0: bool) -> None:
    """DepthTest(arg0: bool) -> None
"""
