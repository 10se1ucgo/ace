import ace
from typing import *
from typing import Iterable as iterable, Iterator as iterator
import ace.glm
import ace.font
import ace.map
__all__  = [
"BlockSelection",
"BoxSelection",
"FaceSelection",
"LineSelection",
"MapEditor",
"kv6_to_blocks"
]
class BlockSelection():
    @overload
    def build(self, arg0: ace.glm.u8vec3, arg1: ace.map.Face, arg2: int = 1) -> None:
        """build(*args, **kwargs)
Overloaded function.

1. build(self: ace.editor.BlockSelection, arg0: ace.glm.u8vec3) -> None

2. build(self: ace.editor.BlockSelection, arg0: ace.glm.u8vec3, arg1: ace.map.Face, arg2: int = 1) -> None
"""

    @overload
    def build(self, arg0: ace.glm.u8vec3) -> None: ...

    @overload
    def destroy(self, arg0: ace.map.Face) -> None: 
        """destroy(*args, **kwargs)
Overloaded function.

1. destroy(self: ace.editor.BlockSelection) -> None

2. destroy(self: ace.editor.BlockSelection, arg0: ace.map.Face) -> None
"""

    @overload
    def destroy(self) -> None: ...

    def get_blocks(self) -> List[ace.glm.ivec3]: 
        """get_blocks(self: ace.editor.BlockSelection) -> List[ace.glm.ivec3]
"""

    @overload
    def paint(self, arg0: ace.glm.u8vec3, arg1: ace.map.Face) -> None: 
        """paint(*args, **kwargs)
Overloaded function.

1. paint(self: ace.editor.BlockSelection, arg0: ace.glm.u8vec3) -> None

2. paint(self: ace.editor.BlockSelection, arg0: ace.glm.u8vec3, arg1: ace.map.Face) -> None
"""

    @overload
    def paint(self, arg0: ace.glm.u8vec3) -> None: ...

    pass

class BoxSelection(BlockSelection):
    @overload
    def __init__(self, arg0: ace.map.AceMap) -> None: 
        """__init__(*args, **kwargs)
Overloaded function.

1. __init__(self: ace.editor.BoxSelection, arg0: ace.map.AceMap) -> None

2. __init__(self: ace.editor.BoxSelection, arg0: ace.map.AceMap, arg1: ace.glm.ivec3, arg2: ace.glm.ivec3) -> None
"""

    @overload
    def __init__(self, arg0: ace.map.AceMap, arg1: ace.glm.ivec3, arg2: ace.glm.ivec3) -> None: ...

    @overload
    def build(self, arg0: ace.glm.u8vec3, arg1: ace.map.Face, arg2: int = 1) -> None:
        """build(*args, **kwargs)
Overloaded function.

1. build(self: ace.editor.BlockSelection, arg0: ace.glm.u8vec3) -> None

2. build(self: ace.editor.BlockSelection, arg0: ace.glm.u8vec3, arg1: ace.map.Face, arg2: int = 1) -> None
"""

    @overload
    def build(self, arg0: ace.glm.u8vec3) -> None: ...

    @overload
    def destroy(self, arg0: ace.map.Face) -> None: 
        """destroy(*args, **kwargs)
Overloaded function.

1. destroy(self: ace.editor.BlockSelection) -> None

2. destroy(self: ace.editor.BlockSelection, arg0: ace.map.Face) -> None
"""

    @overload
    def destroy(self) -> None: ...

    def get_blocks(self) -> List[ace.glm.ivec3]: 
        """get_blocks(self: ace.editor.BlockSelection) -> List[ace.glm.ivec3]
"""

    @overload
    def paint(self, arg0: ace.glm.u8vec3, arg1: ace.map.Face) -> None: 
        """paint(*args, **kwargs)
Overloaded function.

1. paint(self: ace.editor.BlockSelection, arg0: ace.glm.u8vec3) -> None

2. paint(self: ace.editor.BlockSelection, arg0: ace.glm.u8vec3, arg1: ace.map.Face) -> None
"""

    @overload
    def paint(self, arg0: ace.glm.u8vec3) -> None: ...

    def set(self, arg0: ace.glm.ivec3, arg1: ace.glm.ivec3) -> None: 
        """set(self: ace.editor.BoxSelection, arg0: ace.glm.ivec3, arg1: ace.glm.ivec3) -> None
"""

    pass

class FaceSelection(BlockSelection):
    def __init__(self, arg0: ace.map.AceMap) -> None: 
        """__init__(self: ace.editor.FaceSelection, arg0: ace.map.AceMap) -> None
"""

    @overload
    def build(self, arg0: ace.glm.u8vec3, arg1: ace.map.Face, arg2: int = 1) -> None:
        """build(*args, **kwargs)
Overloaded function.

1. build(self: ace.editor.BlockSelection, arg0: ace.glm.u8vec3) -> None

2. build(self: ace.editor.BlockSelection, arg0: ace.glm.u8vec3, arg1: ace.map.Face) -> None
"""

    @overload
    def build(self, arg0: ace.glm.u8vec3) -> None: ...

    @overload
    def destroy(self, arg0: ace.map.Face) -> None: 
        """destroy(*args, **kwargs)
Overloaded function.

1. destroy(self: ace.editor.BlockSelection) -> None

2. destroy(self: ace.editor.BlockSelection, arg0: ace.map.Face) -> None
"""

    @overload
    def destroy(self) -> None: ...

    def get_blocks(self) -> List[ace.glm.ivec3]: 
        """get_blocks(self: ace.editor.BlockSelection) -> List[ace.glm.ivec3]
"""

    @overload
    def paint(self, arg0: ace.glm.u8vec3, arg1: ace.map.Face) -> None: 
        """paint(*args, **kwargs)
Overloaded function.

1. paint(self: ace.editor.BlockSelection, arg0: ace.glm.u8vec3) -> None

2. paint(self: ace.editor.BlockSelection, arg0: ace.glm.u8vec3, arg1: ace.map.Face) -> None
"""

    @overload
    def paint(self, arg0: ace.glm.u8vec3) -> None: ...

    def set(self, arg0: ace.glm.ivec3, arg1: ace.map.Face) -> None: 
        """set(self: ace.editor.FaceSelection, arg0: ace.glm.ivec3, arg1: ace.map.Face) -> None
"""

    pass

class LineSelection(BlockSelection):
    @overload
    def __init__(self, arg0: ace.map.AceMap, arg1: ace.glm.ivec3, arg2: ace.glm.ivec3) -> None: 
        """__init__(*args, **kwargs)
Overloaded function.

1. __init__(self: ace.editor.LineSelection, arg0: ace.map.AceMap) -> None

2. __init__(self: ace.editor.LineSelection, arg0: ace.map.AceMap, arg1: ace.glm.ivec3, arg2: ace.glm.ivec3) -> None
"""

    @overload
    def __init__(self, arg0: ace.map.AceMap) -> None: ...

    @overload
    def build(self, arg0: ace.glm.u8vec3, arg1: ace.map.Face, arg2: int = 1) -> None:
        """build(*args, **kwargs)
Overloaded function.

1. build(self: ace.editor.BlockSelection, arg0: ace.glm.u8vec3) -> None

2. build(self: ace.editor.BlockSelection, arg0: ace.glm.u8vec3, arg1: ace.map.Face, arg2: int = 1) -> None
"""

    @overload
    def build(self, arg0: ace.glm.u8vec3) -> None: ...

    @overload
    def destroy(self, arg0: ace.map.Face) -> None: 
        """destroy(*args, **kwargs)
Overloaded function.

1. destroy(self: ace.editor.BlockSelection) -> None

2. destroy(self: ace.editor.BlockSelection, arg0: ace.map.Face) -> None
"""

    @overload
    def destroy(self) -> None: ...

    def get_blocks(self) -> List[ace.glm.ivec3]: 
        """get_blocks(self: ace.editor.BlockSelection) -> List[ace.glm.ivec3]
"""

    @overload
    def paint(self, arg0: ace.glm.u8vec3, arg1: ace.map.Face) -> None: 
        """paint(*args, **kwargs)
Overloaded function.

1. paint(self: ace.editor.BlockSelection, arg0: ace.glm.u8vec3) -> None

2. paint(self: ace.editor.BlockSelection, arg0: ace.glm.u8vec3, arg1: ace.map.Face) -> None
"""

    @overload
    def paint(self, arg0: ace.glm.u8vec3) -> None: ...

    def set(self, arg0: ace.glm.ivec3, arg1: ace.glm.ivec3) -> None: 
        """set(self: ace.editor.LineSelection, arg0: ace.glm.ivec3, arg1: ace.glm.ivec3) -> None
"""

    pass

class MapEditor():
    def fonts(self, arg0: str, arg1: int) -> ace.font.Font: 
        """fonts(self: ace.editor.MapEditor, arg0: str, arg1: int) -> ace.font.Font
"""

    def hitcast(self) -> tuple: 
        """hitcast(self: ace.editor.MapEditor) -> tuple
"""

    def save_to(self, arg0: str) -> None: 
        """save_to(self: ace.editor.MapEditor, arg0: str) -> None
"""

    @property
    def client(self) -> ace.Client:
        """
:type: ace.GameClient"""
    @property
    def debug(self) -> ace.draw.DebugDraw:
        """
:type: ace.draw.DebugDraw"""
    @property
    def map(self) -> ace.map.AceMap:
        """
:type: ace.map.AceMap"""
    @property
    def uniforms(self) -> ace.gl.Uniforms3D:
        """
:type: ace.gl.Uniforms3D"""
    pass

def kv6_to_blocks(arg0: str) -> Dict[ace.glm.ivec3, ace.glm.u8vec3]:
    """kv6_to_blocks(arg0: str) -> Dict[ace.glm.ivec3, ace.glm.u8vec3]
"""
