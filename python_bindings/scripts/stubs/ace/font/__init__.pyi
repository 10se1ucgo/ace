import ace
from typing import *
from typing import Iterable as iterable, Iterator as iterator
import ace.glm
import enum
__all__  = [
"Align",
"Font",
"Label"
]
class Align(enum.IntEnum, int, enum.Enum):
    """An enumeration."""
    BOTTOM_CENTER = 6
    BOTTOM_LEFT = 1
    BOTTOM_RIGHT = 3
    CENTER = 4
    TOP_CENTER = 5
    TOP_LEFT = 0
    TOP_RIGHT = 2
    pass

class Font():
    @property
    def size(self) -> int:
        ...

    def get_aligned_position(self: ace.font.Font, arg0: ace.glm.vec2, arg1: ace.glm.vec2, arg2: ace.draw.Align) -> ace.glm.vec2:
        ...

    def measure(self: ace.font.Font, arg0: str, arg1: ace.glm.vec2) -> ace.glm.vec2:
        ...

    def draw(self: ace.font.Font, text: str, pos: ace.glm.vec2, color = ace.glm.vec3(1.0), scale = ace.glm.vec2(1.0), align = Align.BOTTOM_LEFT):
        ...

    def draw_shadowed(self: ace.font.Font, text: str, pos: ace.glm.vec2, color = ace.glm.vec3(1.0), scale = ace.glm.vec2(1.0), align = Align.BOTTOM_LEFT):
        ...


class Label():
    def draw(self) -> None: 
        """draw(self: ace.font.Label) -> None
"""

    def draw_shadowed(self) -> None: 
        """draw_shadowed(self: ace.font.Label) -> None
"""

    @overload
    def get_aligned(self, arg0: Align) -> ace.glm.vec2: 
        """get_aligned(*args, **kwargs)
Overloaded function.

1. get_aligned(self: ace.font.Label) -> ace.glm.vec2

2. get_aligned(self: ace.font.Label, arg0: Align) -> ace.glm.vec2
"""

    @overload
    def get_aligned(self) -> ace.glm.vec2: ...

    @property
    def align(self) -> ace.draw.Alin:
        """
:type: Align"""
    @align.setter
    def align(self, arg1: Align) -> None:
        ...

    @property
    def color(self) -> ace.glm.vec3:
        """
:type: ace.glm.vec3"""
    @color.setter
    def color(self, arg1: ace.glm.vec3) -> None:
        ...

    @property
    def position(self) -> ace.glm.vec2:
        """
:type: ace.glm.vec2"""
    @position.setter
    def position(self, arg0: ace.glm.vec2) -> None:
        ...

    @property
    def scale(self) -> ace.glm.vec2:
        """
:type: ace.glm.vec2"""
    @scale.setter
    def scale(self, arg1: ace.glm.vec2) -> None:
        ...

    @property
    def text(self) -> str:
        """
:type: str"""
    @text.setter
    def text(self, arg1: str) -> None:
        ...

    pass
