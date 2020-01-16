"""Python bindings to the OpenGL Mathematics library"""
import ace
from typing import *
from typing import Iterable as iterable, Iterator as iterator
__all__  = [
"ivec2",
"ivec3",
"ivec4",
"mat3",
"mat4",
"u8vec2",
"u8vec3",
"u8vec4",
"vec2",
"vec3",
"vec4",
"distance",
"dot",
"length",
"max",
"min",
"normalize"
]
class ivec2():
    @overload
    def __add__(self, arg0: int) -> ivec2: 
        """__add__(*args, **kwargs)
Overloaded function.

1. __add__(self: ace.glm.ivec2, arg0: ace.glm.ivec2) -> ace.glm.ivec2

2. __add__(self: ace.glm.ivec2, arg0: int) -> ace.glm.ivec2
"""

    @overload
    def __add__(self, arg0: ivec2) -> ivec2: ...

    @overload
    def __and__(self, arg0: int) -> ivec2: 
        """__and__(*args, **kwargs)
Overloaded function.

1. __and__(self: ace.glm.ivec2, arg0: ace.glm.ivec2) -> ace.glm.ivec2

2. __and__(self: ace.glm.ivec2, arg0: int) -> ace.glm.ivec2
"""

    @overload
    def __and__(self, arg0: ivec2) -> ivec2: ...

    def __eq__(self, arg0: ivec2) -> bool: 
        """__eq__(self: ace.glm.ivec2, arg0: ace.glm.ivec2) -> bool
"""

    def __getitem__(self, arg0: int) -> int: 
        """__getitem__(self: ace.glm.ivec2, arg0: int) -> int
"""

    def __hash__(self) -> int: 
        """__hash__(self: ace.glm.ivec2) -> int
"""

    @overload
    def __iadd__(self, arg0: ivec2) -> ivec2: 
        """__iadd__(*args, **kwargs)
Overloaded function.

1. __iadd__(self: ace.glm.ivec2, arg0: ace.glm.ivec2) -> ace.glm.ivec2

2. __iadd__(self: ace.glm.ivec2, arg0: int) -> ace.glm.ivec2
"""

    @overload
    def __iadd__(self, arg0: int) -> ivec2: ...

    @overload
    def __iand__(self, arg0: int) -> ivec2: 
        """__iand__(*args, **kwargs)
Overloaded function.

1. __iand__(self: ace.glm.ivec2, arg0: ace.glm.ivec2) -> ace.glm.ivec2

2. __iand__(self: ace.glm.ivec2, arg0: int) -> ace.glm.ivec2
"""

    @overload
    def __iand__(self, arg0: ivec2) -> ivec2: ...

    @overload
    def __ilshift__(self, arg0: int) -> ivec2: 
        """__ilshift__(*args, **kwargs)
Overloaded function.

1. __ilshift__(self: ace.glm.ivec2, arg0: ace.glm.ivec2) -> ace.glm.ivec2

2. __ilshift__(self: ace.glm.ivec2, arg0: int) -> ace.glm.ivec2
"""

    @overload
    def __ilshift__(self, arg0: ivec2) -> ivec2: ...

    @overload
    def __imul__(self, arg0: ivec2) -> ivec2: 
        """__imul__(*args, **kwargs)
Overloaded function.

1. __imul__(self: ace.glm.ivec2, arg0: ace.glm.ivec2) -> ace.glm.ivec2

2. __imul__(self: ace.glm.ivec2, arg0: int) -> ace.glm.ivec2
"""

    @overload
    def __imul__(self, arg0: int) -> ivec2: ...

    @overload
    def __init__(self, arg0: u8vec2) -> None: 
        """__init__(*args, **kwargs)
Overloaded function.

1. __init__(self: ace.glm.ivec2, x: int, y: int) -> None

2. __init__(self: ace.glm.ivec2, x: int=0) -> None

3. __init__(self: ace.glm.ivec2, arg0: ace.glm.ivec2) -> None

4. __init__(self: ace.glm.ivec2, arg0: ace.glm.vec2) -> None

5. __init__(self: ace.glm.ivec2, arg0: ace.glm.vec3) -> None

6. __init__(self: ace.glm.ivec2, arg0: ace.glm.vec4) -> None

7. __init__(self: ace.glm.ivec2, arg0: ace.glm.ivec3) -> None

8. __init__(self: ace.glm.ivec2, arg0: ace.glm.ivec4) -> None

9. __init__(self: ace.glm.ivec2, arg0: ace.glm.u8vec2) -> None

10. __init__(self: ace.glm.ivec2, arg0: ace.glm.u8vec3) -> None

11. __init__(self: ace.glm.ivec2, arg0: ace.glm.u8vec4) -> None
"""

    @overload
    def __init__(self, arg0: ivec3) -> None: ...

    @overload
    def __init__(self, arg0: u8vec4) -> None: ...

    @overload
    def __init__(self, x: int=0) -> None: ...

    @overload
    def __init__(self, arg0: u8vec3) -> None: ...

    @overload
    def __init__(self, arg0: ivec4) -> None: ...

    @overload
    def __init__(self, x: int, y: int) -> None: ...

    @overload
    def __init__(self, arg0: vec2) -> None: ...

    @overload
    def __init__(self, arg0: ivec2) -> None: ...

    @overload
    def __init__(self, arg0: vec3) -> None: ...

    @overload
    def __init__(self, arg0: vec4) -> None: ...

    @overload
    def __ior__(self, arg0: ivec2) -> ivec2: 
        """__ior__(*args, **kwargs)
Overloaded function.

1. __ior__(self: ace.glm.ivec2, arg0: ace.glm.ivec2) -> ace.glm.ivec2

2. __ior__(self: ace.glm.ivec2, arg0: int) -> ace.glm.ivec2
"""

    @overload
    def __ior__(self, arg0: int) -> ivec2: ...

    @overload
    def __irshift__(self, arg0: ivec2) -> ivec2: 
        """__irshift__(*args, **kwargs)
Overloaded function.

1. __irshift__(self: ace.glm.ivec2, arg0: ace.glm.ivec2) -> ace.glm.ivec2

2. __irshift__(self: ace.glm.ivec2, arg0: int) -> ace.glm.ivec2
"""

    @overload
    def __irshift__(self, arg0: int) -> ivec2: ...

    @overload
    def __isub__(self, arg0: int) -> ivec2: 
        """__isub__(*args, **kwargs)
Overloaded function.

1. __isub__(self: ace.glm.ivec2, arg0: ace.glm.ivec2) -> ace.glm.ivec2

2. __isub__(self: ace.glm.ivec2, arg0: int) -> ace.glm.ivec2
"""

    @overload
    def __isub__(self, arg0: ivec2) -> ivec2: ...

    @overload
    def __itruediv__(self, arg0: int) -> ivec2: 
        """__itruediv__(*args, **kwargs)
Overloaded function.

1. __itruediv__(self: ace.glm.ivec2, arg0: ace.glm.ivec2) -> ace.glm.ivec2

2. __itruediv__(self: ace.glm.ivec2, arg0: int) -> ace.glm.ivec2
"""

    @overload
    def __itruediv__(self, arg0: ivec2) -> ivec2: ...

    @overload
    def __ixor__(self, arg0: ivec2) -> ivec2: 
        """__ixor__(*args, **kwargs)
Overloaded function.

1. __ixor__(self: ace.glm.ivec2, arg0: ace.glm.ivec2) -> ace.glm.ivec2

2. __ixor__(self: ace.glm.ivec2, arg0: int) -> ace.glm.ivec2
"""

    @overload
    def __ixor__(self, arg0: int) -> ivec2: ...

    def __len__(self) -> int: 
        """__len__(self: ace.glm.ivec2) -> int
"""

    @overload
    def __lshift__(self, arg0: ivec2) -> ivec2: 
        """__lshift__(*args, **kwargs)
Overloaded function.

1. __lshift__(self: ace.glm.ivec2, arg0: ace.glm.ivec2) -> ace.glm.ivec2

2. __lshift__(self: ace.glm.ivec2, arg0: int) -> ace.glm.ivec2
"""

    @overload
    def __lshift__(self, arg0: int) -> ivec2: ...

    @overload
    def __mul__(self, arg0: ivec2) -> ivec2: 
        """__mul__(*args, **kwargs)
Overloaded function.

1. __mul__(self: ace.glm.ivec2, arg0: ace.glm.ivec2) -> ace.glm.ivec2

2. __mul__(self: ace.glm.ivec2, arg0: int) -> ace.glm.ivec2
"""

    @overload
    def __mul__(self, arg0: int) -> ivec2: ...

    def __ne__(self, arg0: ivec2) -> bool: 
        """__ne__(self: ace.glm.ivec2, arg0: ace.glm.ivec2) -> bool
"""

    @overload
    def __or__(self, arg0: int) -> ivec2: 
        """__or__(*args, **kwargs)
Overloaded function.

1. __or__(self: ace.glm.ivec2, arg0: ace.glm.ivec2) -> ace.glm.ivec2

2. __or__(self: ace.glm.ivec2, arg0: int) -> ace.glm.ivec2
"""

    @overload
    def __or__(self, arg0: ivec2) -> ivec2: ...

    def __radd__(self, arg0: int) -> ivec2: 
        """__radd__(self: ace.glm.ivec2, arg0: int) -> ace.glm.ivec2
"""

    def __rand__(self, arg0: int) -> ivec2: 
        """__rand__(self: ace.glm.ivec2, arg0: int) -> ace.glm.ivec2
"""

    def __repr__(self) -> str: 
        """__repr__(self: ace.glm.ivec2) -> str
"""

    def __rlshift__(self, arg0: int) -> ivec2: 
        """__rlshift__(self: ace.glm.ivec2, arg0: int) -> ace.glm.ivec2
"""

    def __rmul__(self, arg0: int) -> ivec2: 
        """__rmul__(self: ace.glm.ivec2, arg0: int) -> ace.glm.ivec2
"""

    def __ror__(self, arg0: int) -> ivec2: 
        """__ror__(self: ace.glm.ivec2, arg0: int) -> ace.glm.ivec2
"""

    def __rrshift__(self, arg0: int) -> ivec2: 
        """__rrshift__(self: ace.glm.ivec2, arg0: int) -> ace.glm.ivec2
"""

    @overload
    def __rshift__(self, arg0: ivec2) -> ivec2: 
        """__rshift__(*args, **kwargs)
Overloaded function.

1. __rshift__(self: ace.glm.ivec2, arg0: ace.glm.ivec2) -> ace.glm.ivec2

2. __rshift__(self: ace.glm.ivec2, arg0: int) -> ace.glm.ivec2
"""

    @overload
    def __rshift__(self, arg0: int) -> ivec2: ...

    def __rsub__(self, arg0: int) -> ivec2: 
        """__rsub__(self: ace.glm.ivec2, arg0: int) -> ace.glm.ivec2
"""

    def __rtruediv__(self, arg0: int) -> ivec2: 
        """__rtruediv__(self: ace.glm.ivec2, arg0: int) -> ace.glm.ivec2
"""

    def __rxor__(self, arg0: int) -> ivec2: 
        """__rxor__(self: ace.glm.ivec2, arg0: int) -> ace.glm.ivec2
"""

    def __setitem__(self, arg0: int, arg1: int) -> None: 
        """__setitem__(self: ace.glm.ivec2, arg0: int, arg1: int) -> None
"""

    @overload
    def __sub__(self, arg0: ivec2) -> ivec2: 
        """__sub__(*args, **kwargs)
Overloaded function.

1. __sub__(self: ace.glm.ivec2, arg0: ace.glm.ivec2) -> ace.glm.ivec2

2. __sub__(self: ace.glm.ivec2, arg0: int) -> ace.glm.ivec2
"""

    @overload
    def __sub__(self, arg0: int) -> ivec2: ...

    @overload
    def __truediv__(self, arg0: int) -> ivec2: 
        """__truediv__(*args, **kwargs)
Overloaded function.

1. __truediv__(self: ace.glm.ivec2, arg0: ace.glm.ivec2) -> ace.glm.ivec2

2. __truediv__(self: ace.glm.ivec2, arg0: int) -> ace.glm.ivec2
"""

    @overload
    def __truediv__(self, arg0: ivec2) -> ivec2: ...

    @overload
    def __xor__(self, arg0: ivec2) -> ivec2: 
        """__xor__(*args, **kwargs)
Overloaded function.

1. __xor__(self: ace.glm.ivec2, arg0: ace.glm.ivec2) -> ace.glm.ivec2

2. __xor__(self: ace.glm.ivec2, arg0: int) -> ace.glm.ivec2
"""

    @overload
    def __xor__(self, arg0: int) -> ivec2: ...

    @property
    def g(self) -> int:
        """
:type: int"""
    @g.setter
    def g(self, arg0: int) -> None:
        ...

    @property
    def r(self) -> int:
        """
:type: int"""
    @r.setter
    def r(self, arg0: int) -> None:
        ...

    @property
    def s(self) -> int:
        """
:type: int"""
    @s.setter
    def s(self, arg0: int) -> None:
        ...

    @property
    def t(self) -> int:
        """
:type: int"""
    @t.setter
    def t(self, arg0: int) -> None:
        ...

    @property
    def x(self) -> int:
        """
:type: int"""
    @x.setter
    def x(self, arg0: int) -> None:
        ...

    @property
    def y(self) -> int:
        """
:type: int"""
    @y.setter
    def y(self, arg0: int) -> None:
        ...

    pass

class ivec3():
    @overload
    def __add__(self, arg0: int) -> ivec3: 
        """__add__(*args, **kwargs)
Overloaded function.

1. __add__(self: ace.glm.ivec3, arg0: ace.glm.ivec3) -> ace.glm.ivec3

2. __add__(self: ace.glm.ivec3, arg0: int) -> ace.glm.ivec3
"""

    @overload
    def __add__(self, arg0: ivec3) -> ivec3: ...

    @overload
    def __and__(self, arg0: int) -> ivec3: 
        """__and__(*args, **kwargs)
Overloaded function.

1. __and__(self: ace.glm.ivec3, arg0: ace.glm.ivec3) -> ace.glm.ivec3

2. __and__(self: ace.glm.ivec3, arg0: int) -> ace.glm.ivec3
"""

    @overload
    def __and__(self, arg0: ivec3) -> ivec3: ...

    def __eq__(self, arg0: ivec3) -> bool: 
        """__eq__(self: ace.glm.ivec3, arg0: ace.glm.ivec3) -> bool
"""

    def __getitem__(self, arg0: int) -> int: 
        """__getitem__(self: ace.glm.ivec3, arg0: int) -> int
"""

    def __hash__(self) -> int: 
        """__hash__(self: ace.glm.ivec3) -> int
"""

    @overload
    def __iadd__(self, arg0: ivec3) -> ivec3: 
        """__iadd__(*args, **kwargs)
Overloaded function.

1. __iadd__(self: ace.glm.ivec3, arg0: ace.glm.ivec3) -> ace.glm.ivec3

2. __iadd__(self: ace.glm.ivec3, arg0: int) -> ace.glm.ivec3
"""

    @overload
    def __iadd__(self, arg0: int) -> ivec3: ...

    @overload
    def __iand__(self, arg0: int) -> ivec3: 
        """__iand__(*args, **kwargs)
Overloaded function.

1. __iand__(self: ace.glm.ivec3, arg0: ace.glm.ivec3) -> ace.glm.ivec3

2. __iand__(self: ace.glm.ivec3, arg0: int) -> ace.glm.ivec3
"""

    @overload
    def __iand__(self, arg0: ivec3) -> ivec3: ...

    @overload
    def __ilshift__(self, arg0: ivec3) -> ivec3: 
        """__ilshift__(*args, **kwargs)
Overloaded function.

1. __ilshift__(self: ace.glm.ivec3, arg0: ace.glm.ivec3) -> ace.glm.ivec3

2. __ilshift__(self: ace.glm.ivec3, arg0: int) -> ace.glm.ivec3
"""

    @overload
    def __ilshift__(self, arg0: int) -> ivec3: ...

    @overload
    def __imul__(self, arg0: ivec3) -> ivec3: 
        """__imul__(*args, **kwargs)
Overloaded function.

1. __imul__(self: ace.glm.ivec3, arg0: ace.glm.ivec3) -> ace.glm.ivec3

2. __imul__(self: ace.glm.ivec3, arg0: int) -> ace.glm.ivec3
"""

    @overload
    def __imul__(self, arg0: int) -> ivec3: ...

    @overload
    def __init__(self, x: int, y: int, z: int) -> None: 
        """__init__(*args, **kwargs)
Overloaded function.

1. __init__(self: ace.glm.ivec3, x: int, y: int, z: int) -> None

2. __init__(self: ace.glm.ivec3, x: int=0) -> None

3. __init__(self: ace.glm.ivec3, arg0: ace.glm.ivec3) -> None

4. __init__(self: ace.glm.ivec3, arg0: ace.glm.vec2, arg1: int) -> None

5. __init__(self: ace.glm.ivec3, arg0: ace.glm.vec3) -> None

6. __init__(self: ace.glm.ivec3, arg0: ace.glm.vec4) -> None

7. __init__(self: ace.glm.ivec3, arg0: ace.glm.ivec2, arg1: int) -> None

8. __init__(self: ace.glm.ivec3, arg0: ace.glm.ivec4) -> None

9. __init__(self: ace.glm.ivec3, arg0: ace.glm.u8vec2, arg1: int) -> None

10. __init__(self: ace.glm.ivec3, arg0: ace.glm.u8vec3) -> None

11. __init__(self: ace.glm.ivec3, arg0: ace.glm.u8vec4) -> None
"""

    @overload
    def __init__(self, arg0: vec4) -> None: ...

    @overload
    def __init__(self, arg0: ivec2, arg1: int) -> None: ...

    @overload
    def __init__(self, arg0: vec2, arg1: int) -> None: ...

    @overload
    def __init__(self, arg0: ivec3) -> None: ...

    @overload
    def __init__(self, arg0: ivec4) -> None: ...

    @overload
    def __init__(self, arg0: u8vec4) -> None: ...

    @overload
    def __init__(self, arg0: u8vec2, arg1: int) -> None: ...

    @overload
    def __init__(self, x: int=0) -> None: ...

    @overload
    def __init__(self, arg0: vec3) -> None: ...

    @overload
    def __init__(self, arg0: u8vec3) -> None: ...

    @overload
    def __ior__(self, arg0: int) -> ivec3: 
        """__ior__(*args, **kwargs)
Overloaded function.

1. __ior__(self: ace.glm.ivec3, arg0: ace.glm.ivec3) -> ace.glm.ivec3

2. __ior__(self: ace.glm.ivec3, arg0: int) -> ace.glm.ivec3
"""

    @overload
    def __ior__(self, arg0: ivec3) -> ivec3: ...

    @overload
    def __irshift__(self, arg0: int) -> ivec3: 
        """__irshift__(*args, **kwargs)
Overloaded function.

1. __irshift__(self: ace.glm.ivec3, arg0: ace.glm.ivec3) -> ace.glm.ivec3

2. __irshift__(self: ace.glm.ivec3, arg0: int) -> ace.glm.ivec3
"""

    @overload
    def __irshift__(self, arg0: ivec3) -> ivec3: ...

    @overload
    def __isub__(self, arg0: ivec3) -> ivec3: 
        """__isub__(*args, **kwargs)
Overloaded function.

1. __isub__(self: ace.glm.ivec3, arg0: ace.glm.ivec3) -> ace.glm.ivec3

2. __isub__(self: ace.glm.ivec3, arg0: int) -> ace.glm.ivec3
"""

    @overload
    def __isub__(self, arg0: int) -> ivec3: ...

    @overload
    def __itruediv__(self, arg0: ivec3) -> ivec3: 
        """__itruediv__(*args, **kwargs)
Overloaded function.

1. __itruediv__(self: ace.glm.ivec3, arg0: ace.glm.ivec3) -> ace.glm.ivec3

2. __itruediv__(self: ace.glm.ivec3, arg0: int) -> ace.glm.ivec3
"""

    @overload
    def __itruediv__(self, arg0: int) -> ivec3: ...

    @overload
    def __ixor__(self, arg0: int) -> ivec3: 
        """__ixor__(*args, **kwargs)
Overloaded function.

1. __ixor__(self: ace.glm.ivec3, arg0: ace.glm.ivec3) -> ace.glm.ivec3

2. __ixor__(self: ace.glm.ivec3, arg0: int) -> ace.glm.ivec3
"""

    @overload
    def __ixor__(self, arg0: ivec3) -> ivec3: ...

    def __len__(self) -> int: 
        """__len__(self: ace.glm.ivec3) -> int
"""

    @overload
    def __lshift__(self, arg0: ivec3) -> ivec3: 
        """__lshift__(*args, **kwargs)
Overloaded function.

1. __lshift__(self: ace.glm.ivec3, arg0: ace.glm.ivec3) -> ace.glm.ivec3

2. __lshift__(self: ace.glm.ivec3, arg0: int) -> ace.glm.ivec3
"""

    @overload
    def __lshift__(self, arg0: int) -> ivec3: ...

    @overload
    def __mul__(self, arg0: int) -> ivec3: 
        """__mul__(*args, **kwargs)
Overloaded function.

1. __mul__(self: ace.glm.ivec3, arg0: ace.glm.ivec3) -> ace.glm.ivec3

2. __mul__(self: ace.glm.ivec3, arg0: int) -> ace.glm.ivec3
"""

    @overload
    def __mul__(self, arg0: ivec3) -> ivec3: ...

    def __ne__(self, arg0: ivec3) -> bool: 
        """__ne__(self: ace.glm.ivec3, arg0: ace.glm.ivec3) -> bool
"""

    @overload
    def __or__(self, arg0: int) -> ivec3: 
        """__or__(*args, **kwargs)
Overloaded function.

1. __or__(self: ace.glm.ivec3, arg0: ace.glm.ivec3) -> ace.glm.ivec3

2. __or__(self: ace.glm.ivec3, arg0: int) -> ace.glm.ivec3
"""

    @overload
    def __or__(self, arg0: ivec3) -> ivec3: ...

    def __radd__(self, arg0: int) -> ivec3: 
        """__radd__(self: ace.glm.ivec3, arg0: int) -> ace.glm.ivec3
"""

    def __rand__(self, arg0: int) -> ivec3: 
        """__rand__(self: ace.glm.ivec3, arg0: int) -> ace.glm.ivec3
"""

    def __repr__(self) -> str: 
        """__repr__(self: ace.glm.ivec3) -> str
"""

    def __rlshift__(self, arg0: int) -> ivec3: 
        """__rlshift__(self: ace.glm.ivec3, arg0: int) -> ace.glm.ivec3
"""

    def __rmul__(self, arg0: int) -> ivec3: 
        """__rmul__(self: ace.glm.ivec3, arg0: int) -> ace.glm.ivec3
"""

    def __ror__(self, arg0: int) -> ivec3: 
        """__ror__(self: ace.glm.ivec3, arg0: int) -> ace.glm.ivec3
"""

    def __rrshift__(self, arg0: int) -> ivec3: 
        """__rrshift__(self: ace.glm.ivec3, arg0: int) -> ace.glm.ivec3
"""

    @overload
    def __rshift__(self, arg0: ivec3) -> ivec3: 
        """__rshift__(*args, **kwargs)
Overloaded function.

1. __rshift__(self: ace.glm.ivec3, arg0: ace.glm.ivec3) -> ace.glm.ivec3

2. __rshift__(self: ace.glm.ivec3, arg0: int) -> ace.glm.ivec3
"""

    @overload
    def __rshift__(self, arg0: int) -> ivec3: ...

    def __rsub__(self, arg0: int) -> ivec3: 
        """__rsub__(self: ace.glm.ivec3, arg0: int) -> ace.glm.ivec3
"""

    def __rtruediv__(self, arg0: int) -> ivec3: 
        """__rtruediv__(self: ace.glm.ivec3, arg0: int) -> ace.glm.ivec3
"""

    def __rxor__(self, arg0: int) -> ivec3: 
        """__rxor__(self: ace.glm.ivec3, arg0: int) -> ace.glm.ivec3
"""

    def __setitem__(self, arg0: int, arg1: int) -> None: 
        """__setitem__(self: ace.glm.ivec3, arg0: int, arg1: int) -> None
"""

    @overload
    def __sub__(self, arg0: int) -> ivec3: 
        """__sub__(*args, **kwargs)
Overloaded function.

1. __sub__(self: ace.glm.ivec3, arg0: ace.glm.ivec3) -> ace.glm.ivec3

2. __sub__(self: ace.glm.ivec3, arg0: int) -> ace.glm.ivec3
"""

    @overload
    def __sub__(self, arg0: ivec3) -> ivec3: ...

    @overload
    def __truediv__(self, arg0: ivec3) -> ivec3: 
        """__truediv__(*args, **kwargs)
Overloaded function.

1. __truediv__(self: ace.glm.ivec3, arg0: ace.glm.ivec3) -> ace.glm.ivec3

2. __truediv__(self: ace.glm.ivec3, arg0: int) -> ace.glm.ivec3
"""

    @overload
    def __truediv__(self, arg0: int) -> ivec3: ...

    @overload
    def __xor__(self, arg0: int) -> ivec3: 
        """__xor__(*args, **kwargs)
Overloaded function.

1. __xor__(self: ace.glm.ivec3, arg0: ace.glm.ivec3) -> ace.glm.ivec3

2. __xor__(self: ace.glm.ivec3, arg0: int) -> ace.glm.ivec3
"""

    @overload
    def __xor__(self, arg0: ivec3) -> ivec3: ...

    @property
    def b(self) -> int:
        """
:type: int"""
    @b.setter
    def b(self, arg0: int) -> None:
        ...

    @property
    def g(self) -> int:
        """
:type: int"""
    @g.setter
    def g(self, arg0: int) -> None:
        ...

    @property
    def p(self) -> int:
        """
:type: int"""
    @p.setter
    def p(self, arg0: int) -> None:
        ...

    @property
    def r(self) -> int:
        """
:type: int"""
    @r.setter
    def r(self, arg0: int) -> None:
        ...

    @property
    def s(self) -> int:
        """
:type: int"""
    @s.setter
    def s(self, arg0: int) -> None:
        ...

    @property
    def t(self) -> int:
        """
:type: int"""
    @t.setter
    def t(self, arg0: int) -> None:
        ...

    @property
    def x(self) -> int:
        """
:type: int"""
    @x.setter
    def x(self, arg0: int) -> None:
        ...

    @property
    def y(self) -> int:
        """
:type: int"""
    @y.setter
    def y(self, arg0: int) -> None:
        ...

    @property
    def z(self) -> int:
        """
:type: int"""
    @z.setter
    def z(self, arg0: int) -> None:
        ...

    pass

class ivec4():
    @overload
    def __add__(self, arg0: ivec4) -> ivec4: 
        """__add__(*args, **kwargs)
Overloaded function.

1. __add__(self: ace.glm.ivec4, arg0: ace.glm.ivec4) -> ace.glm.ivec4

2. __add__(self: ace.glm.ivec4, arg0: int) -> ace.glm.ivec4
"""

    @overload
    def __add__(self, arg0: int) -> ivec4: ...

    @overload
    def __and__(self, arg0: int) -> ivec4: 
        """__and__(*args, **kwargs)
Overloaded function.

1. __and__(self: ace.glm.ivec4, arg0: ace.glm.ivec4) -> ace.glm.ivec4

2. __and__(self: ace.glm.ivec4, arg0: int) -> ace.glm.ivec4
"""

    @overload
    def __and__(self, arg0: ivec4) -> ivec4: ...

    def __eq__(self, arg0: ivec4) -> bool: 
        """__eq__(self: ace.glm.ivec4, arg0: ace.glm.ivec4) -> bool
"""

    def __getitem__(self, arg0: int) -> int: 
        """__getitem__(self: ace.glm.ivec4, arg0: int) -> int
"""

    def __hash__(self) -> int: 
        """__hash__(self: ace.glm.ivec4) -> int
"""

    @overload
    def __iadd__(self, arg0: int) -> ivec4: 
        """__iadd__(*args, **kwargs)
Overloaded function.

1. __iadd__(self: ace.glm.ivec4, arg0: ace.glm.ivec4) -> ace.glm.ivec4

2. __iadd__(self: ace.glm.ivec4, arg0: int) -> ace.glm.ivec4
"""

    @overload
    def __iadd__(self, arg0: ivec4) -> ivec4: ...

    @overload
    def __iand__(self, arg0: ivec4) -> ivec4: 
        """__iand__(*args, **kwargs)
Overloaded function.

1. __iand__(self: ace.glm.ivec4, arg0: ace.glm.ivec4) -> ace.glm.ivec4

2. __iand__(self: ace.glm.ivec4, arg0: int) -> ace.glm.ivec4
"""

    @overload
    def __iand__(self, arg0: int) -> ivec4: ...

    @overload
    def __ilshift__(self, arg0: int) -> ivec4: 
        """__ilshift__(*args, **kwargs)
Overloaded function.

1. __ilshift__(self: ace.glm.ivec4, arg0: ace.glm.ivec4) -> ace.glm.ivec4

2. __ilshift__(self: ace.glm.ivec4, arg0: int) -> ace.glm.ivec4
"""

    @overload
    def __ilshift__(self, arg0: ivec4) -> ivec4: ...

    @overload
    def __imul__(self, arg0: int) -> ivec4: 
        """__imul__(*args, **kwargs)
Overloaded function.

1. __imul__(self: ace.glm.ivec4, arg0: ace.glm.ivec4) -> ace.glm.ivec4

2. __imul__(self: ace.glm.ivec4, arg0: int) -> ace.glm.ivec4
"""

    @overload
    def __imul__(self, arg0: ivec4) -> ivec4: ...

    @overload
    def __init__(self, arg0: u8vec3, arg1: int) -> None: 
        """__init__(*args, **kwargs)
Overloaded function.

1. __init__(self: ace.glm.ivec4, x: int, y: int, z: int, w: int) -> None

2. __init__(self: ace.glm.ivec4, arg0: int) -> None

3. __init__(self: ace.glm.ivec4, arg0: ace.glm.ivec4) -> None

4. __init__(self: ace.glm.ivec4, arg0: ace.glm.vec2, arg1: int, arg2: int) -> None

5. __init__(self: ace.glm.ivec4, arg0: ace.glm.vec3, arg1: int) -> None

6. __init__(self: ace.glm.ivec4, arg0: ace.glm.vec4) -> None

7. __init__(self: ace.glm.ivec4, arg0: ace.glm.ivec2, arg1: int, arg2: int) -> None

8. __init__(self: ace.glm.ivec4, arg0: ace.glm.ivec3, arg1: int) -> None

9. __init__(self: ace.glm.ivec4, arg0: ace.glm.u8vec2, arg1: int, arg2: int) -> None

10. __init__(self: ace.glm.ivec4, arg0: ace.glm.u8vec3, arg1: int) -> None

11. __init__(self: ace.glm.ivec4, arg0: ace.glm.u8vec4) -> None
"""

    @overload
    def __init__(self, arg0: vec4) -> None: ...

    @overload
    def __init__(self, arg0: ivec2, arg1: int, arg2: int) -> None: ...

    @overload
    def __init__(self, arg0: ivec3, arg1: int) -> None: ...

    @overload
    def __init__(self, arg0: u8vec4) -> None: ...

    @overload
    def __init__(self, arg0: ivec4) -> None: ...

    @overload
    def __init__(self, arg0: int) -> None: ...

    @overload
    def __init__(self, x: int, y: int, z: int, w: int) -> None: ...

    @overload
    def __init__(self, arg0: vec2, arg1: int, arg2: int) -> None: ...

    @overload
    def __init__(self, arg0: u8vec2, arg1: int, arg2: int) -> None: ...

    @overload
    def __init__(self, arg0: vec3, arg1: int) -> None: ...

    @overload
    def __ior__(self, arg0: ivec4) -> ivec4: 
        """__ior__(*args, **kwargs)
Overloaded function.

1. __ior__(self: ace.glm.ivec4, arg0: ace.glm.ivec4) -> ace.glm.ivec4

2. __ior__(self: ace.glm.ivec4, arg0: int) -> ace.glm.ivec4
"""

    @overload
    def __ior__(self, arg0: int) -> ivec4: ...

    @overload
    def __irshift__(self, arg0: ivec4) -> ivec4: 
        """__irshift__(*args, **kwargs)
Overloaded function.

1. __irshift__(self: ace.glm.ivec4, arg0: ace.glm.ivec4) -> ace.glm.ivec4

2. __irshift__(self: ace.glm.ivec4, arg0: int) -> ace.glm.ivec4
"""

    @overload
    def __irshift__(self, arg0: int) -> ivec4: ...

    @overload
    def __isub__(self, arg0: int) -> ivec4: 
        """__isub__(*args, **kwargs)
Overloaded function.

1. __isub__(self: ace.glm.ivec4, arg0: ace.glm.ivec4) -> ace.glm.ivec4

2. __isub__(self: ace.glm.ivec4, arg0: int) -> ace.glm.ivec4
"""

    @overload
    def __isub__(self, arg0: ivec4) -> ivec4: ...

    @overload
    def __itruediv__(self, arg0: int) -> ivec4: 
        """__itruediv__(*args, **kwargs)
Overloaded function.

1. __itruediv__(self: ace.glm.ivec4, arg0: ace.glm.ivec4) -> ace.glm.ivec4

2. __itruediv__(self: ace.glm.ivec4, arg0: int) -> ace.glm.ivec4
"""

    @overload
    def __itruediv__(self, arg0: ivec4) -> ivec4: ...

    @overload
    def __ixor__(self, arg0: int) -> ivec4: 
        """__ixor__(*args, **kwargs)
Overloaded function.

1. __ixor__(self: ace.glm.ivec4, arg0: ace.glm.ivec4) -> ace.glm.ivec4

2. __ixor__(self: ace.glm.ivec4, arg0: int) -> ace.glm.ivec4
"""

    @overload
    def __ixor__(self, arg0: ivec4) -> ivec4: ...

    def __len__(self) -> int: 
        """__len__(self: ace.glm.ivec4) -> int
"""

    @overload
    def __lshift__(self, arg0: ivec4) -> ivec4: 
        """__lshift__(*args, **kwargs)
Overloaded function.

1. __lshift__(self: ace.glm.ivec4, arg0: ace.glm.ivec4) -> ace.glm.ivec4

2. __lshift__(self: ace.glm.ivec4, arg0: int) -> ace.glm.ivec4
"""

    @overload
    def __lshift__(self, arg0: int) -> ivec4: ...

    @overload
    def __mul__(self, arg0: ivec4) -> ivec4: 
        """__mul__(*args, **kwargs)
Overloaded function.

1. __mul__(self: ace.glm.ivec4, arg0: ace.glm.ivec4) -> ace.glm.ivec4

2. __mul__(self: ace.glm.ivec4, arg0: int) -> ace.glm.ivec4
"""

    @overload
    def __mul__(self, arg0: int) -> ivec4: ...

    def __ne__(self, arg0: ivec4) -> bool: 
        """__ne__(self: ace.glm.ivec4, arg0: ace.glm.ivec4) -> bool
"""

    @overload
    def __or__(self, arg0: int) -> ivec4: 
        """__or__(*args, **kwargs)
Overloaded function.

1. __or__(self: ace.glm.ivec4, arg0: ace.glm.ivec4) -> ace.glm.ivec4

2. __or__(self: ace.glm.ivec4, arg0: int) -> ace.glm.ivec4
"""

    @overload
    def __or__(self, arg0: ivec4) -> ivec4: ...

    def __radd__(self, arg0: int) -> ivec4: 
        """__radd__(self: ace.glm.ivec4, arg0: int) -> ace.glm.ivec4
"""

    def __rand__(self, arg0: int) -> ivec4: 
        """__rand__(self: ace.glm.ivec4, arg0: int) -> ace.glm.ivec4
"""

    def __repr__(self) -> str: 
        """__repr__(self: ace.glm.ivec4) -> str
"""

    def __rlshift__(self, arg0: int) -> ivec4: 
        """__rlshift__(self: ace.glm.ivec4, arg0: int) -> ace.glm.ivec4
"""

    def __rmul__(self, arg0: int) -> ivec4: 
        """__rmul__(self: ace.glm.ivec4, arg0: int) -> ace.glm.ivec4
"""

    def __ror__(self, arg0: int) -> ivec4: 
        """__ror__(self: ace.glm.ivec4, arg0: int) -> ace.glm.ivec4
"""

    def __rrshift__(self, arg0: int) -> ivec4: 
        """__rrshift__(self: ace.glm.ivec4, arg0: int) -> ace.glm.ivec4
"""

    @overload
    def __rshift__(self, arg0: int) -> ivec4: 
        """__rshift__(*args, **kwargs)
Overloaded function.

1. __rshift__(self: ace.glm.ivec4, arg0: ace.glm.ivec4) -> ace.glm.ivec4

2. __rshift__(self: ace.glm.ivec4, arg0: int) -> ace.glm.ivec4
"""

    @overload
    def __rshift__(self, arg0: ivec4) -> ivec4: ...

    def __rsub__(self, arg0: int) -> ivec4: 
        """__rsub__(self: ace.glm.ivec4, arg0: int) -> ace.glm.ivec4
"""

    def __rtruediv__(self, arg0: int) -> ivec4: 
        """__rtruediv__(self: ace.glm.ivec4, arg0: int) -> ace.glm.ivec4
"""

    def __rxor__(self, arg0: int) -> ivec4: 
        """__rxor__(self: ace.glm.ivec4, arg0: int) -> ace.glm.ivec4
"""

    def __setitem__(self, arg0: int, arg1: int) -> None: 
        """__setitem__(self: ace.glm.ivec4, arg0: int, arg1: int) -> None
"""

    @overload
    def __sub__(self, arg0: ivec4) -> ivec4: 
        """__sub__(*args, **kwargs)
Overloaded function.

1. __sub__(self: ace.glm.ivec4, arg0: ace.glm.ivec4) -> ace.glm.ivec4

2. __sub__(self: ace.glm.ivec4, arg0: int) -> ace.glm.ivec4
"""

    @overload
    def __sub__(self, arg0: int) -> ivec4: ...

    @overload
    def __truediv__(self, arg0: int) -> ivec4: 
        """__truediv__(*args, **kwargs)
Overloaded function.

1. __truediv__(self: ace.glm.ivec4, arg0: ace.glm.ivec4) -> ace.glm.ivec4

2. __truediv__(self: ace.glm.ivec4, arg0: int) -> ace.glm.ivec4
"""

    @overload
    def __truediv__(self, arg0: ivec4) -> ivec4: ...

    @overload
    def __xor__(self, arg0: int) -> ivec4: 
        """__xor__(*args, **kwargs)
Overloaded function.

1. __xor__(self: ace.glm.ivec4, arg0: ace.glm.ivec4) -> ace.glm.ivec4

2. __xor__(self: ace.glm.ivec4, arg0: int) -> ace.glm.ivec4
"""

    @overload
    def __xor__(self, arg0: ivec4) -> ivec4: ...

    @property
    def a(self) -> int:
        """
:type: int"""
    @a.setter
    def a(self, arg0: int) -> None:
        ...

    @property
    def b(self) -> int:
        """
:type: int"""
    @b.setter
    def b(self, arg0: int) -> None:
        ...

    @property
    def g(self) -> int:
        """
:type: int"""
    @g.setter
    def g(self, arg0: int) -> None:
        ...

    @property
    def p(self) -> int:
        """
:type: int"""
    @p.setter
    def p(self, arg0: int) -> None:
        ...

    @property
    def q(self) -> int:
        """
:type: int"""
    @q.setter
    def q(self, arg0: int) -> None:
        ...

    @property
    def r(self) -> int:
        """
:type: int"""
    @r.setter
    def r(self, arg0: int) -> None:
        ...

    @property
    def s(self) -> int:
        """
:type: int"""
    @s.setter
    def s(self, arg0: int) -> None:
        ...

    @property
    def t(self) -> int:
        """
:type: int"""
    @t.setter
    def t(self, arg0: int) -> None:
        ...

    @property
    def w(self) -> int:
        """
:type: int"""
    @w.setter
    def w(self, arg0: int) -> None:
        ...

    @property
    def x(self) -> int:
        """
:type: int"""
    @x.setter
    def x(self, arg0: int) -> None:
        ...

    @property
    def y(self) -> int:
        """
:type: int"""
    @y.setter
    def y(self, arg0: int) -> None:
        ...

    @property
    def z(self) -> int:
        """
:type: int"""
    @z.setter
    def z(self, arg0: int) -> None:
        ...

    pass

class mat3():
    @overload
    def __add__(self, arg0: mat3) -> mat3: 
        """__add__(*args, **kwargs)
Overloaded function.

1. __add__(self: ace.glm.mat3, arg0: ace.glm.mat3) -> ace.glm.mat3

2. __add__(self: ace.glm.mat3, arg0: float) -> ace.glm.mat3
"""

    @overload
    def __add__(self, arg0: float) -> mat3: ...

    @overload
    def __getitem__(self, arg0: int) -> vec3: 
        """__getitem__(*args, **kwargs)
Overloaded function.

1. __getitem__(self: ace.glm.mat3, arg0: int) -> ace.glm.vec3

2. __getitem__(self: ace.glm.mat3, arg0: Tuple[int, int]) -> float
"""

    @overload
    def __getitem__(self, arg0: Tuple[int, int]) -> float: ...

    def __hash__(self) -> int: 
        """__hash__(self: ace.glm.mat3) -> int
"""

    @overload
    def __iadd__(self, arg0: float) -> mat3: 
        """__iadd__(*args, **kwargs)
Overloaded function.

1. __iadd__(self: ace.glm.mat3, arg0: ace.glm.mat3) -> ace.glm.mat3

2. __iadd__(self: ace.glm.mat3, arg0: float) -> ace.glm.mat3
"""

    @overload
    def __iadd__(self, arg0: mat3) -> mat3: ...

    @overload
    def __imul__(self, arg0: float) -> mat3: 
        """__imul__(*args, **kwargs)
Overloaded function.

1. __imul__(self: ace.glm.mat3, arg0: ace.glm.mat3) -> ace.glm.mat3

2. __imul__(self: ace.glm.mat3, arg0: float) -> ace.glm.mat3
"""

    @overload
    def __imul__(self, arg0: mat3) -> mat3: ...

    @overload
    def __init__(self, x: float=1) -> None: 
        """__init__(*args, **kwargs)
Overloaded function.

1. __init__(self: ace.glm.mat3, x: float=1) -> None

2. __init__(self: ace.glm.mat3, arg0: float, arg1: float, arg2: float, arg3: float, arg4: float, arg5: float, arg6: float, arg7: float, arg8: float) -> None

3. __init__(self: ace.glm.mat3, arg0: ace.glm.vec3, arg1: ace.glm.vec3, arg2: ace.glm.vec3) -> None

4. __init__(self: ace.glm.mat3, arg0: ace.glm.mat3) -> None
"""

    @overload
    def __init__(self, arg0: vec3, arg1: vec3, arg2: vec3) -> None: ...

    @overload
    def __init__(self, arg0: mat3) -> None: ...

    @overload
    def __init__(self, arg0: float, arg1: float, arg2: float, arg3: float, arg4: float, arg5: float, arg6: float, arg7: float, arg8: float) -> None: ...

    @overload
    def __isub__(self, arg0: mat3) -> mat3: 
        """__isub__(*args, **kwargs)
Overloaded function.

1. __isub__(self: ace.glm.mat3, arg0: ace.glm.mat3) -> ace.glm.mat3

2. __isub__(self: ace.glm.mat3, arg0: float) -> ace.glm.mat3
"""

    @overload
    def __isub__(self, arg0: float) -> mat3: ...

    @overload
    def __itruediv__(self, arg0: mat3) -> mat3: 
        """__itruediv__(*args, **kwargs)
Overloaded function.

1. __itruediv__(self: ace.glm.mat3, arg0: ace.glm.mat3) -> ace.glm.mat3

2. __itruediv__(self: ace.glm.mat3, arg0: float) -> ace.glm.mat3
"""

    @overload
    def __itruediv__(self, arg0: float) -> mat3: ...

    def __len__(self) -> int: 
        """__len__(self: ace.glm.mat3) -> int
"""

    @overload
    def __mul__(self, arg0: float) -> mat3: 
        """__mul__(*args, **kwargs)
Overloaded function.

1. __mul__(self: ace.glm.mat3, arg0: ace.glm.mat3) -> ace.glm.mat3

2. __mul__(self: ace.glm.mat3, arg0: float) -> ace.glm.mat3

3. __mul__(self: ace.glm.mat3, arg0: ace.glm.vec3) -> ace.glm.vec3
"""

    @overload
    def __mul__(self, arg0: vec3) -> vec3: ...

    @overload
    def __mul__(self, arg0: mat3) -> mat3: ...

    def __radd__(self, arg0: float) -> mat3: 
        """__radd__(self: ace.glm.mat3, arg0: float) -> ace.glm.mat3
"""

    def __repr__(self) -> str: 
        """__repr__(self: ace.glm.mat3) -> str
"""

    @overload
    def __rmul__(self, arg0: vec3) -> vec3: 
        """__rmul__(*args, **kwargs)
Overloaded function.

1. __rmul__(self: ace.glm.mat3, arg0: float) -> ace.glm.mat3

2. __rmul__(self: ace.glm.mat3, arg0: ace.glm.vec3) -> ace.glm.vec3
"""

    @overload
    def __rmul__(self, arg0: float) -> mat3: ...

    def __rsub__(self, arg0: float) -> mat3: 
        """__rsub__(self: ace.glm.mat3, arg0: float) -> ace.glm.mat3
"""

    @overload
    def __rtruediv__(self, arg0: float) -> mat3: 
        """__rtruediv__(*args, **kwargs)
Overloaded function.

1. __rtruediv__(self: ace.glm.mat3, arg0: float) -> ace.glm.mat3

2. __rtruediv__(self: ace.glm.mat3, arg0: ace.glm.vec3) -> ace.glm.vec3
"""

    @overload
    def __rtruediv__(self, arg0: vec3) -> vec3: ...

    @overload
    def __setitem__(self, arg0: int, arg1: vec3) -> None: 
        """__setitem__(*args, **kwargs)
Overloaded function.

1. __setitem__(self: ace.glm.mat3, arg0: int, arg1: ace.glm.vec3) -> None

2. __setitem__(self: ace.glm.mat3, arg0: Tuple[int, int], arg1: float) -> None
"""

    @overload
    def __setitem__(self, arg0: Tuple[int, int], arg1: float) -> None: ...

    @overload
    def __sub__(self, arg0: float) -> mat3: 
        """__sub__(*args, **kwargs)
Overloaded function.

1. __sub__(self: ace.glm.mat3, arg0: ace.glm.mat3) -> ace.glm.mat3

2. __sub__(self: ace.glm.mat3, arg0: float) -> ace.glm.mat3
"""

    @overload
    def __sub__(self, arg0: mat3) -> mat3: ...

    @overload
    def __truediv__(self, arg0: vec3) -> vec3: 
        """__truediv__(*args, **kwargs)
Overloaded function.

1. __truediv__(self: ace.glm.mat3, arg0: ace.glm.mat3) -> ace.glm.mat3

2. __truediv__(self: ace.glm.mat3, arg0: float) -> ace.glm.mat3

3. __truediv__(self: ace.glm.mat3, arg0: ace.glm.vec3) -> ace.glm.vec3
"""

    @overload
    def __truediv__(self, arg0: mat3) -> mat3: ...

    @overload
    def __truediv__(self, arg0: float) -> mat3: ...

    pass

class mat4():
    @overload
    def __add__(self, arg0: float) -> mat4: 
        """__add__(*args, **kwargs)
Overloaded function.

1. __add__(self: ace.glm.mat4, arg0: ace.glm.mat4) -> ace.glm.mat4

2. __add__(self: ace.glm.mat4, arg0: float) -> ace.glm.mat4
"""

    @overload
    def __add__(self, arg0: mat4) -> mat4: ...

    @overload
    def __getitem__(self, arg0: Tuple[int, int]) -> float: 
        """__getitem__(*args, **kwargs)
Overloaded function.

1. __getitem__(self: ace.glm.mat4, arg0: int) -> ace.glm.vec4

2. __getitem__(self: ace.glm.mat4, arg0: Tuple[int, int]) -> float
"""

    @overload
    def __getitem__(self, arg0: int) -> vec4: ...

    def __hash__(self) -> int: 
        """__hash__(self: ace.glm.mat4) -> int
"""

    @overload
    def __iadd__(self, arg0: mat4) -> mat4: 
        """__iadd__(*args, **kwargs)
Overloaded function.

1. __iadd__(self: ace.glm.mat4, arg0: ace.glm.mat4) -> ace.glm.mat4

2. __iadd__(self: ace.glm.mat4, arg0: float) -> ace.glm.mat4
"""

    @overload
    def __iadd__(self, arg0: float) -> mat4: ...

    @overload
    def __imul__(self, arg0: float) -> mat4: 
        """__imul__(*args, **kwargs)
Overloaded function.

1. __imul__(self: ace.glm.mat4, arg0: ace.glm.mat4) -> ace.glm.mat4

2. __imul__(self: ace.glm.mat4, arg0: float) -> ace.glm.mat4
"""

    @overload
    def __imul__(self, arg0: mat4) -> mat4: ...

    @overload
    def __init__(self, arg0: vec4, arg1: vec4, arg2: vec4, arg3: vec4) -> None: 
        """__init__(*args, **kwargs)
Overloaded function.

1. __init__(self: ace.glm.mat4, x: float=1) -> None

2. __init__(self: ace.glm.mat4, arg0: float, arg1: float, arg2: float, arg3: float, arg4: float, arg5: float, arg6: float, arg7: float, arg8: float, arg9: float, arg10: float, arg11: float, arg12: float, arg13: float, arg14: float, arg15: float) -> None

3. __init__(self: ace.glm.mat4, arg0: ace.glm.vec4, arg1: ace.glm.vec4, arg2: ace.glm.vec4, arg3: ace.glm.vec4) -> None

4. __init__(self: ace.glm.mat4, arg0: ace.glm.mat4) -> None
"""

    @overload
    def __init__(self, arg0: mat4) -> None: ...

    @overload
    def __init__(self, x: float=1) -> None: ...

    @overload
    def __init__(self, arg0: float, arg1: float, arg2: float, arg3: float, arg4: float, arg5: float, arg6: float, arg7: float, arg8: float, arg9: float, arg10: float, arg11: float, arg12: float, arg13: float, arg14: float, arg15: float) -> None: ...

    @overload
    def __isub__(self, arg0: mat4) -> mat4: 
        """__isub__(*args, **kwargs)
Overloaded function.

1. __isub__(self: ace.glm.mat4, arg0: ace.glm.mat4) -> ace.glm.mat4

2. __isub__(self: ace.glm.mat4, arg0: float) -> ace.glm.mat4
"""

    @overload
    def __isub__(self, arg0: float) -> mat4: ...

    @overload
    def __itruediv__(self, arg0: mat4) -> mat4: 
        """__itruediv__(*args, **kwargs)
Overloaded function.

1. __itruediv__(self: ace.glm.mat4, arg0: ace.glm.mat4) -> ace.glm.mat4

2. __itruediv__(self: ace.glm.mat4, arg0: float) -> ace.glm.mat4
"""

    @overload
    def __itruediv__(self, arg0: float) -> mat4: ...

    def __len__(self) -> int: 
        """__len__(self: ace.glm.mat4) -> int
"""

    @overload
    def __mul__(self, arg0: vec4) -> vec4: 
        """__mul__(*args, **kwargs)
Overloaded function.

1. __mul__(self: ace.glm.mat4, arg0: ace.glm.mat4) -> ace.glm.mat4

2. __mul__(self: ace.glm.mat4, arg0: float) -> ace.glm.mat4

3. __mul__(self: ace.glm.mat4, arg0: ace.glm.vec4) -> ace.glm.vec4
"""

    @overload
    def __mul__(self, arg0: float) -> mat4: ...

    @overload
    def __mul__(self, arg0: mat4) -> mat4: ...

    def __radd__(self, arg0: float) -> mat4: 
        """__radd__(self: ace.glm.mat4, arg0: float) -> ace.glm.mat4
"""

    def __repr__(self) -> str: 
        """__repr__(self: ace.glm.mat4) -> str
"""

    @overload
    def __rmul__(self, arg0: float) -> mat4: 
        """__rmul__(*args, **kwargs)
Overloaded function.

1. __rmul__(self: ace.glm.mat4, arg0: float) -> ace.glm.mat4

2. __rmul__(self: ace.glm.mat4, arg0: ace.glm.vec4) -> ace.glm.vec4
"""

    @overload
    def __rmul__(self, arg0: vec4) -> vec4: ...

    def __rsub__(self, arg0: float) -> mat4: 
        """__rsub__(self: ace.glm.mat4, arg0: float) -> ace.glm.mat4
"""

    @overload
    def __rtruediv__(self, arg0: vec4) -> vec4: 
        """__rtruediv__(*args, **kwargs)
Overloaded function.

1. __rtruediv__(self: ace.glm.mat4, arg0: float) -> ace.glm.mat4

2. __rtruediv__(self: ace.glm.mat4, arg0: ace.glm.vec4) -> ace.glm.vec4
"""

    @overload
    def __rtruediv__(self, arg0: float) -> mat4: ...

    @overload
    def __setitem__(self, arg0: Tuple[int, int], arg1: float) -> None: 
        """__setitem__(*args, **kwargs)
Overloaded function.

1. __setitem__(self: ace.glm.mat4, arg0: int, arg1: ace.glm.vec4) -> None

2. __setitem__(self: ace.glm.mat4, arg0: Tuple[int, int], arg1: float) -> None
"""

    @overload
    def __setitem__(self, arg0: int, arg1: vec4) -> None: ...

    @overload
    def __sub__(self, arg0: mat4) -> mat4: 
        """__sub__(*args, **kwargs)
Overloaded function.

1. __sub__(self: ace.glm.mat4, arg0: ace.glm.mat4) -> ace.glm.mat4

2. __sub__(self: ace.glm.mat4, arg0: float) -> ace.glm.mat4
"""

    @overload
    def __sub__(self, arg0: float) -> mat4: ...

    @overload
    def __truediv__(self, arg0: mat4) -> mat4: 
        """__truediv__(*args, **kwargs)
Overloaded function.

1. __truediv__(self: ace.glm.mat4, arg0: ace.glm.mat4) -> ace.glm.mat4

2. __truediv__(self: ace.glm.mat4, arg0: float) -> ace.glm.mat4

3. __truediv__(self: ace.glm.mat4, arg0: ace.glm.vec4) -> ace.glm.vec4
"""

    @overload
    def __truediv__(self, arg0: float) -> mat4: ...

    @overload
    def __truediv__(self, arg0: vec4) -> vec4: ...

    pass

class u8vec2():
    @overload
    def __add__(self, arg0: int) -> u8vec2: 
        """__add__(*args, **kwargs)
Overloaded function.

1. __add__(self: ace.glm.u8vec2, arg0: ace.glm.u8vec2) -> ace.glm.u8vec2

2. __add__(self: ace.glm.u8vec2, arg0: int) -> ace.glm.u8vec2
"""

    @overload
    def __add__(self, arg0: u8vec2) -> u8vec2: ...

    @overload
    def __and__(self, arg0: u8vec2) -> u8vec2: 
        """__and__(*args, **kwargs)
Overloaded function.

1. __and__(self: ace.glm.u8vec2, arg0: ace.glm.u8vec2) -> ace.glm.u8vec2

2. __and__(self: ace.glm.u8vec2, arg0: int) -> ace.glm.u8vec2
"""

    @overload
    def __and__(self, arg0: int) -> u8vec2: ...

    def __eq__(self, arg0: u8vec2) -> bool: 
        """__eq__(self: ace.glm.u8vec2, arg0: ace.glm.u8vec2) -> bool
"""

    def __getitem__(self, arg0: int) -> int: 
        """__getitem__(self: ace.glm.u8vec2, arg0: int) -> int
"""

    def __hash__(self) -> int: 
        """__hash__(self: ace.glm.u8vec2) -> int
"""

    @overload
    def __iadd__(self, arg0: u8vec2) -> u8vec2: 
        """__iadd__(*args, **kwargs)
Overloaded function.

1. __iadd__(self: ace.glm.u8vec2, arg0: ace.glm.u8vec2) -> ace.glm.u8vec2

2. __iadd__(self: ace.glm.u8vec2, arg0: int) -> ace.glm.u8vec2
"""

    @overload
    def __iadd__(self, arg0: int) -> u8vec2: ...

    @overload
    def __iand__(self, arg0: u8vec2) -> u8vec2: 
        """__iand__(*args, **kwargs)
Overloaded function.

1. __iand__(self: ace.glm.u8vec2, arg0: ace.glm.u8vec2) -> ace.glm.u8vec2

2. __iand__(self: ace.glm.u8vec2, arg0: int) -> ace.glm.u8vec2
"""

    @overload
    def __iand__(self, arg0: int) -> u8vec2: ...

    @overload
    def __ilshift__(self, arg0: int) -> u8vec2: 
        """__ilshift__(*args, **kwargs)
Overloaded function.

1. __ilshift__(self: ace.glm.u8vec2, arg0: ace.glm.u8vec2) -> ace.glm.u8vec2

2. __ilshift__(self: ace.glm.u8vec2, arg0: int) -> ace.glm.u8vec2
"""

    @overload
    def __ilshift__(self, arg0: u8vec2) -> u8vec2: ...

    @overload
    def __imul__(self, arg0: int) -> u8vec2: 
        """__imul__(*args, **kwargs)
Overloaded function.

1. __imul__(self: ace.glm.u8vec2, arg0: ace.glm.u8vec2) -> ace.glm.u8vec2

2. __imul__(self: ace.glm.u8vec2, arg0: int) -> ace.glm.u8vec2
"""

    @overload
    def __imul__(self, arg0: u8vec2) -> u8vec2: ...

    @overload
    def __init__(self, arg0: ivec4) -> None: 
        """__init__(*args, **kwargs)
Overloaded function.

1. __init__(self: ace.glm.u8vec2, x: int, y: int) -> None

2. __init__(self: ace.glm.u8vec2, x: int=0) -> None

3. __init__(self: ace.glm.u8vec2, arg0: ace.glm.u8vec2) -> None

4. __init__(self: ace.glm.u8vec2, arg0: ace.glm.vec2) -> None

5. __init__(self: ace.glm.u8vec2, arg0: ace.glm.vec3) -> None

6. __init__(self: ace.glm.u8vec2, arg0: ace.glm.vec4) -> None

7. __init__(self: ace.glm.u8vec2, arg0: ace.glm.ivec2) -> None

8. __init__(self: ace.glm.u8vec2, arg0: ace.glm.ivec3) -> None

9. __init__(self: ace.glm.u8vec2, arg0: ace.glm.ivec4) -> None

10. __init__(self: ace.glm.u8vec2, arg0: ace.glm.u8vec3) -> None

11. __init__(self: ace.glm.u8vec2, arg0: ace.glm.u8vec4) -> None
"""

    @overload
    def __init__(self, arg0: vec3) -> None: ...

    @overload
    def __init__(self, arg0: ivec3) -> None: ...

    @overload
    def __init__(self, arg0: ivec2) -> None: ...

    @overload
    def __init__(self, x: int=0) -> None: ...

    @overload
    def __init__(self, arg0: u8vec3) -> None: ...

    @overload
    def __init__(self, arg0: u8vec2) -> None: ...

    @overload
    def __init__(self, arg0: vec4) -> None: ...

    @overload
    def __init__(self, arg0: vec2) -> None: ...

    @overload
    def __init__(self, x: int, y: int) -> None: ...

    @overload
    def __init__(self, arg0: u8vec4) -> None: ...

    @overload
    def __ior__(self, arg0: int) -> u8vec2: 
        """__ior__(*args, **kwargs)
Overloaded function.

1. __ior__(self: ace.glm.u8vec2, arg0: ace.glm.u8vec2) -> ace.glm.u8vec2

2. __ior__(self: ace.glm.u8vec2, arg0: int) -> ace.glm.u8vec2
"""

    @overload
    def __ior__(self, arg0: u8vec2) -> u8vec2: ...

    @overload
    def __irshift__(self, arg0: int) -> u8vec2: 
        """__irshift__(*args, **kwargs)
Overloaded function.

1. __irshift__(self: ace.glm.u8vec2, arg0: ace.glm.u8vec2) -> ace.glm.u8vec2

2. __irshift__(self: ace.glm.u8vec2, arg0: int) -> ace.glm.u8vec2
"""

    @overload
    def __irshift__(self, arg0: u8vec2) -> u8vec2: ...

    @overload
    def __isub__(self, arg0: int) -> u8vec2: 
        """__isub__(*args, **kwargs)
Overloaded function.

1. __isub__(self: ace.glm.u8vec2, arg0: ace.glm.u8vec2) -> ace.glm.u8vec2

2. __isub__(self: ace.glm.u8vec2, arg0: int) -> ace.glm.u8vec2
"""

    @overload
    def __isub__(self, arg0: u8vec2) -> u8vec2: ...

    @overload
    def __itruediv__(self, arg0: int) -> u8vec2: 
        """__itruediv__(*args, **kwargs)
Overloaded function.

1. __itruediv__(self: ace.glm.u8vec2, arg0: ace.glm.u8vec2) -> ace.glm.u8vec2

2. __itruediv__(self: ace.glm.u8vec2, arg0: int) -> ace.glm.u8vec2
"""

    @overload
    def __itruediv__(self, arg0: u8vec2) -> u8vec2: ...

    @overload
    def __ixor__(self, arg0: u8vec2) -> u8vec2: 
        """__ixor__(*args, **kwargs)
Overloaded function.

1. __ixor__(self: ace.glm.u8vec2, arg0: ace.glm.u8vec2) -> ace.glm.u8vec2

2. __ixor__(self: ace.glm.u8vec2, arg0: int) -> ace.glm.u8vec2
"""

    @overload
    def __ixor__(self, arg0: int) -> u8vec2: ...

    def __len__(self) -> int: 
        """__len__(self: ace.glm.u8vec2) -> int
"""

    @overload
    def __lshift__(self, arg0: u8vec2) -> u8vec2: 
        """__lshift__(*args, **kwargs)
Overloaded function.

1. __lshift__(self: ace.glm.u8vec2, arg0: ace.glm.u8vec2) -> ace.glm.u8vec2

2. __lshift__(self: ace.glm.u8vec2, arg0: int) -> ace.glm.u8vec2
"""

    @overload
    def __lshift__(self, arg0: int) -> u8vec2: ...

    @overload
    def __mul__(self, arg0: u8vec2) -> u8vec2: 
        """__mul__(*args, **kwargs)
Overloaded function.

1. __mul__(self: ace.glm.u8vec2, arg0: ace.glm.u8vec2) -> ace.glm.u8vec2

2. __mul__(self: ace.glm.u8vec2, arg0: int) -> ace.glm.u8vec2
"""

    @overload
    def __mul__(self, arg0: int) -> u8vec2: ...

    def __ne__(self, arg0: u8vec2) -> bool: 
        """__ne__(self: ace.glm.u8vec2, arg0: ace.glm.u8vec2) -> bool
"""

    @overload
    def __or__(self, arg0: int) -> u8vec2: 
        """__or__(*args, **kwargs)
Overloaded function.

1. __or__(self: ace.glm.u8vec2, arg0: ace.glm.u8vec2) -> ace.glm.u8vec2

2. __or__(self: ace.glm.u8vec2, arg0: int) -> ace.glm.u8vec2
"""

    @overload
    def __or__(self, arg0: u8vec2) -> u8vec2: ...

    def __radd__(self, arg0: int) -> u8vec2: 
        """__radd__(self: ace.glm.u8vec2, arg0: int) -> ace.glm.u8vec2
"""

    def __rand__(self, arg0: int) -> u8vec2: 
        """__rand__(self: ace.glm.u8vec2, arg0: int) -> ace.glm.u8vec2
"""

    def __repr__(self) -> str: 
        """__repr__(self: ace.glm.u8vec2) -> str
"""

    def __rlshift__(self, arg0: int) -> u8vec2: 
        """__rlshift__(self: ace.glm.u8vec2, arg0: int) -> ace.glm.u8vec2
"""

    def __rmul__(self, arg0: int) -> u8vec2: 
        """__rmul__(self: ace.glm.u8vec2, arg0: int) -> ace.glm.u8vec2
"""

    def __ror__(self, arg0: int) -> u8vec2: 
        """__ror__(self: ace.glm.u8vec2, arg0: int) -> ace.glm.u8vec2
"""

    def __rrshift__(self, arg0: int) -> u8vec2: 
        """__rrshift__(self: ace.glm.u8vec2, arg0: int) -> ace.glm.u8vec2
"""

    @overload
    def __rshift__(self, arg0: int) -> u8vec2: 
        """__rshift__(*args, **kwargs)
Overloaded function.

1. __rshift__(self: ace.glm.u8vec2, arg0: ace.glm.u8vec2) -> ace.glm.u8vec2

2. __rshift__(self: ace.glm.u8vec2, arg0: int) -> ace.glm.u8vec2
"""

    @overload
    def __rshift__(self, arg0: u8vec2) -> u8vec2: ...

    def __rsub__(self, arg0: int) -> u8vec2: 
        """__rsub__(self: ace.glm.u8vec2, arg0: int) -> ace.glm.u8vec2
"""

    def __rtruediv__(self, arg0: int) -> u8vec2: 
        """__rtruediv__(self: ace.glm.u8vec2, arg0: int) -> ace.glm.u8vec2
"""

    def __rxor__(self, arg0: int) -> u8vec2: 
        """__rxor__(self: ace.glm.u8vec2, arg0: int) -> ace.glm.u8vec2
"""

    def __setitem__(self, arg0: int, arg1: int) -> None: 
        """__setitem__(self: ace.glm.u8vec2, arg0: int, arg1: int) -> None
"""

    @overload
    def __sub__(self, arg0: int) -> u8vec2: 
        """__sub__(*args, **kwargs)
Overloaded function.

1. __sub__(self: ace.glm.u8vec2, arg0: ace.glm.u8vec2) -> ace.glm.u8vec2

2. __sub__(self: ace.glm.u8vec2, arg0: int) -> ace.glm.u8vec2
"""

    @overload
    def __sub__(self, arg0: u8vec2) -> u8vec2: ...

    @overload
    def __truediv__(self, arg0: int) -> u8vec2: 
        """__truediv__(*args, **kwargs)
Overloaded function.

1. __truediv__(self: ace.glm.u8vec2, arg0: ace.glm.u8vec2) -> ace.glm.u8vec2

2. __truediv__(self: ace.glm.u8vec2, arg0: int) -> ace.glm.u8vec2
"""

    @overload
    def __truediv__(self, arg0: u8vec2) -> u8vec2: ...

    @overload
    def __xor__(self, arg0: u8vec2) -> u8vec2: 
        """__xor__(*args, **kwargs)
Overloaded function.

1. __xor__(self: ace.glm.u8vec2, arg0: ace.glm.u8vec2) -> ace.glm.u8vec2

2. __xor__(self: ace.glm.u8vec2, arg0: int) -> ace.glm.u8vec2
"""

    @overload
    def __xor__(self, arg0: int) -> u8vec2: ...

    @property
    def g(self) -> int:
        """
:type: int"""
    @g.setter
    def g(self, arg0: int) -> None:
        ...

    @property
    def r(self) -> int:
        """
:type: int"""
    @r.setter
    def r(self, arg0: int) -> None:
        ...

    @property
    def s(self) -> int:
        """
:type: int"""
    @s.setter
    def s(self, arg0: int) -> None:
        ...

    @property
    def t(self) -> int:
        """
:type: int"""
    @t.setter
    def t(self, arg0: int) -> None:
        ...

    @property
    def x(self) -> int:
        """
:type: int"""
    @x.setter
    def x(self, arg0: int) -> None:
        ...

    @property
    def y(self) -> int:
        """
:type: int"""
    @y.setter
    def y(self, arg0: int) -> None:
        ...

    pass

class u8vec3():
    @overload
    def __add__(self, arg0: int) -> u8vec3: 
        """__add__(*args, **kwargs)
Overloaded function.

1. __add__(self: ace.glm.u8vec3, arg0: ace.glm.u8vec3) -> ace.glm.u8vec3

2. __add__(self: ace.glm.u8vec3, arg0: int) -> ace.glm.u8vec3
"""

    @overload
    def __add__(self, arg0: u8vec3) -> u8vec3: ...

    @overload
    def __and__(self, arg0: u8vec3) -> u8vec3: 
        """__and__(*args, **kwargs)
Overloaded function.

1. __and__(self: ace.glm.u8vec3, arg0: ace.glm.u8vec3) -> ace.glm.u8vec3

2. __and__(self: ace.glm.u8vec3, arg0: int) -> ace.glm.u8vec3
"""

    @overload
    def __and__(self, arg0: int) -> u8vec3: ...

    def __eq__(self, arg0: u8vec3) -> bool: 
        """__eq__(self: ace.glm.u8vec3, arg0: ace.glm.u8vec3) -> bool
"""

    def __getitem__(self, arg0: int) -> int: 
        """__getitem__(self: ace.glm.u8vec3, arg0: int) -> int
"""

    def __hash__(self) -> int: 
        """__hash__(self: ace.glm.u8vec3) -> int
"""

    @overload
    def __iadd__(self, arg0: u8vec3) -> u8vec3: 
        """__iadd__(*args, **kwargs)
Overloaded function.

1. __iadd__(self: ace.glm.u8vec3, arg0: ace.glm.u8vec3) -> ace.glm.u8vec3

2. __iadd__(self: ace.glm.u8vec3, arg0: int) -> ace.glm.u8vec3
"""

    @overload
    def __iadd__(self, arg0: int) -> u8vec3: ...

    @overload
    def __iand__(self, arg0: u8vec3) -> u8vec3: 
        """__iand__(*args, **kwargs)
Overloaded function.

1. __iand__(self: ace.glm.u8vec3, arg0: ace.glm.u8vec3) -> ace.glm.u8vec3

2. __iand__(self: ace.glm.u8vec3, arg0: int) -> ace.glm.u8vec3
"""

    @overload
    def __iand__(self, arg0: int) -> u8vec3: ...

    @overload
    def __ilshift__(self, arg0: int) -> u8vec3: 
        """__ilshift__(*args, **kwargs)
Overloaded function.

1. __ilshift__(self: ace.glm.u8vec3, arg0: ace.glm.u8vec3) -> ace.glm.u8vec3

2. __ilshift__(self: ace.glm.u8vec3, arg0: int) -> ace.glm.u8vec3
"""

    @overload
    def __ilshift__(self, arg0: u8vec3) -> u8vec3: ...

    @overload
    def __imul__(self, arg0: int) -> u8vec3: 
        """__imul__(*args, **kwargs)
Overloaded function.

1. __imul__(self: ace.glm.u8vec3, arg0: ace.glm.u8vec3) -> ace.glm.u8vec3

2. __imul__(self: ace.glm.u8vec3, arg0: int) -> ace.glm.u8vec3
"""

    @overload
    def __imul__(self, arg0: u8vec3) -> u8vec3: ...

    @overload
    def __init__(self, x: int=0) -> None: 
        """__init__(*args, **kwargs)
Overloaded function.

1. __init__(self: ace.glm.u8vec3, x: int, y: int, z: int) -> None

2. __init__(self: ace.glm.u8vec3, x: int=0) -> None

3. __init__(self: ace.glm.u8vec3, arg0: ace.glm.u8vec3) -> None

4. __init__(self: ace.glm.u8vec3, arg0: ace.glm.vec2, arg1: int) -> None

5. __init__(self: ace.glm.u8vec3, arg0: ace.glm.vec3) -> None

6. __init__(self: ace.glm.u8vec3, arg0: ace.glm.vec4) -> None

7. __init__(self: ace.glm.u8vec3, arg0: ace.glm.ivec2, arg1: int) -> None

8. __init__(self: ace.glm.u8vec3, arg0: ace.glm.ivec3) -> None

9. __init__(self: ace.glm.u8vec3, arg0: ace.glm.ivec4) -> None

10. __init__(self: ace.glm.u8vec3, arg0: ace.glm.u8vec2, arg1: int) -> None

11. __init__(self: ace.glm.u8vec3, arg0: ace.glm.u8vec4) -> None
"""

    @overload
    def __init__(self, arg0: u8vec4) -> None: ...

    @overload
    def __init__(self, arg0: u8vec2, arg1: int) -> None: ...

    @overload
    def __init__(self, arg0: ivec3) -> None: ...

    @overload
    def __init__(self, arg0: vec3) -> None: ...

    @overload
    def __init__(self, arg0: ivec4) -> None: ...

    @overload
    def __init__(self, arg0: u8vec3) -> None: ...

    @overload
    def __init__(self, arg0: ivec2, arg1: int) -> None: ...

    @overload
    def __init__(self, arg0: vec2, arg1: int) -> None: ...

    @overload
    def __init__(self, arg0: vec4) -> None: ...

    @overload
    def __init__(self, x: int, y: int, z: int) -> None: ...

    @overload
    def __ior__(self, arg0: int) -> u8vec3: 
        """__ior__(*args, **kwargs)
Overloaded function.

1. __ior__(self: ace.glm.u8vec3, arg0: ace.glm.u8vec3) -> ace.glm.u8vec3

2. __ior__(self: ace.glm.u8vec3, arg0: int) -> ace.glm.u8vec3
"""

    @overload
    def __ior__(self, arg0: u8vec3) -> u8vec3: ...

    @overload
    def __irshift__(self, arg0: int) -> u8vec3: 
        """__irshift__(*args, **kwargs)
Overloaded function.

1. __irshift__(self: ace.glm.u8vec3, arg0: ace.glm.u8vec3) -> ace.glm.u8vec3

2. __irshift__(self: ace.glm.u8vec3, arg0: int) -> ace.glm.u8vec3
"""

    @overload
    def __irshift__(self, arg0: u8vec3) -> u8vec3: ...

    @overload
    def __isub__(self, arg0: int) -> u8vec3: 
        """__isub__(*args, **kwargs)
Overloaded function.

1. __isub__(self: ace.glm.u8vec3, arg0: ace.glm.u8vec3) -> ace.glm.u8vec3

2. __isub__(self: ace.glm.u8vec3, arg0: int) -> ace.glm.u8vec3
"""

    @overload
    def __isub__(self, arg0: u8vec3) -> u8vec3: ...

    @overload
    def __itruediv__(self, arg0: int) -> u8vec3: 
        """__itruediv__(*args, **kwargs)
Overloaded function.

1. __itruediv__(self: ace.glm.u8vec3, arg0: ace.glm.u8vec3) -> ace.glm.u8vec3

2. __itruediv__(self: ace.glm.u8vec3, arg0: int) -> ace.glm.u8vec3
"""

    @overload
    def __itruediv__(self, arg0: u8vec3) -> u8vec3: ...

    @overload
    def __ixor__(self, arg0: u8vec3) -> u8vec3: 
        """__ixor__(*args, **kwargs)
Overloaded function.

1. __ixor__(self: ace.glm.u8vec3, arg0: ace.glm.u8vec3) -> ace.glm.u8vec3

2. __ixor__(self: ace.glm.u8vec3, arg0: int) -> ace.glm.u8vec3
"""

    @overload
    def __ixor__(self, arg0: int) -> u8vec3: ...

    def __len__(self) -> int: 
        """__len__(self: ace.glm.u8vec3) -> int
"""

    @overload
    def __lshift__(self, arg0: u8vec3) -> u8vec3: 
        """__lshift__(*args, **kwargs)
Overloaded function.

1. __lshift__(self: ace.glm.u8vec3, arg0: ace.glm.u8vec3) -> ace.glm.u8vec3

2. __lshift__(self: ace.glm.u8vec3, arg0: int) -> ace.glm.u8vec3
"""

    @overload
    def __lshift__(self, arg0: int) -> u8vec3: ...

    @overload
    def __mul__(self, arg0: u8vec3) -> u8vec3: 
        """__mul__(*args, **kwargs)
Overloaded function.

1. __mul__(self: ace.glm.u8vec3, arg0: ace.glm.u8vec3) -> ace.glm.u8vec3

2. __mul__(self: ace.glm.u8vec3, arg0: int) -> ace.glm.u8vec3
"""

    @overload
    def __mul__(self, arg0: int) -> u8vec3: ...

    def __ne__(self, arg0: u8vec3) -> bool: 
        """__ne__(self: ace.glm.u8vec3, arg0: ace.glm.u8vec3) -> bool
"""

    @overload
    def __or__(self, arg0: int) -> u8vec3: 
        """__or__(*args, **kwargs)
Overloaded function.

1. __or__(self: ace.glm.u8vec3, arg0: ace.glm.u8vec3) -> ace.glm.u8vec3

2. __or__(self: ace.glm.u8vec3, arg0: int) -> ace.glm.u8vec3
"""

    @overload
    def __or__(self, arg0: u8vec3) -> u8vec3: ...

    def __radd__(self, arg0: int) -> u8vec3: 
        """__radd__(self: ace.glm.u8vec3, arg0: int) -> ace.glm.u8vec3
"""

    def __rand__(self, arg0: int) -> u8vec3: 
        """__rand__(self: ace.glm.u8vec3, arg0: int) -> ace.glm.u8vec3
"""

    def __repr__(self) -> str: 
        """__repr__(self: ace.glm.u8vec3) -> str
"""

    def __rlshift__(self, arg0: int) -> u8vec3: 
        """__rlshift__(self: ace.glm.u8vec3, arg0: int) -> ace.glm.u8vec3
"""

    def __rmul__(self, arg0: int) -> u8vec3: 
        """__rmul__(self: ace.glm.u8vec3, arg0: int) -> ace.glm.u8vec3
"""

    def __ror__(self, arg0: int) -> u8vec3: 
        """__ror__(self: ace.glm.u8vec3, arg0: int) -> ace.glm.u8vec3
"""

    def __rrshift__(self, arg0: int) -> u8vec3: 
        """__rrshift__(self: ace.glm.u8vec3, arg0: int) -> ace.glm.u8vec3
"""

    @overload
    def __rshift__(self, arg0: int) -> u8vec3: 
        """__rshift__(*args, **kwargs)
Overloaded function.

1. __rshift__(self: ace.glm.u8vec3, arg0: ace.glm.u8vec3) -> ace.glm.u8vec3

2. __rshift__(self: ace.glm.u8vec3, arg0: int) -> ace.glm.u8vec3
"""

    @overload
    def __rshift__(self, arg0: u8vec3) -> u8vec3: ...

    def __rsub__(self, arg0: int) -> u8vec3: 
        """__rsub__(self: ace.glm.u8vec3, arg0: int) -> ace.glm.u8vec3
"""

    def __rtruediv__(self, arg0: int) -> u8vec3: 
        """__rtruediv__(self: ace.glm.u8vec3, arg0: int) -> ace.glm.u8vec3
"""

    def __rxor__(self, arg0: int) -> u8vec3: 
        """__rxor__(self: ace.glm.u8vec3, arg0: int) -> ace.glm.u8vec3
"""

    def __setitem__(self, arg0: int, arg1: int) -> None: 
        """__setitem__(self: ace.glm.u8vec3, arg0: int, arg1: int) -> None
"""

    @overload
    def __sub__(self, arg0: int) -> u8vec3: 
        """__sub__(*args, **kwargs)
Overloaded function.

1. __sub__(self: ace.glm.u8vec3, arg0: ace.glm.u8vec3) -> ace.glm.u8vec3

2. __sub__(self: ace.glm.u8vec3, arg0: int) -> ace.glm.u8vec3
"""

    @overload
    def __sub__(self, arg0: u8vec3) -> u8vec3: ...

    @overload
    def __truediv__(self, arg0: int) -> u8vec3: 
        """__truediv__(*args, **kwargs)
Overloaded function.

1. __truediv__(self: ace.glm.u8vec3, arg0: ace.glm.u8vec3) -> ace.glm.u8vec3

2. __truediv__(self: ace.glm.u8vec3, arg0: int) -> ace.glm.u8vec3
"""

    @overload
    def __truediv__(self, arg0: u8vec3) -> u8vec3: ...

    @overload
    def __xor__(self, arg0: u8vec3) -> u8vec3: 
        """__xor__(*args, **kwargs)
Overloaded function.

1. __xor__(self: ace.glm.u8vec3, arg0: ace.glm.u8vec3) -> ace.glm.u8vec3

2. __xor__(self: ace.glm.u8vec3, arg0: int) -> ace.glm.u8vec3
"""

    @overload
    def __xor__(self, arg0: int) -> u8vec3: ...

    @property
    def b(self) -> int:
        """
:type: int"""
    @b.setter
    def b(self, arg0: int) -> None:
        ...

    @property
    def g(self) -> int:
        """
:type: int"""
    @g.setter
    def g(self, arg0: int) -> None:
        ...

    @property
    def p(self) -> int:
        """
:type: int"""
    @p.setter
    def p(self, arg0: int) -> None:
        ...

    @property
    def r(self) -> int:
        """
:type: int"""
    @r.setter
    def r(self, arg0: int) -> None:
        ...

    @property
    def s(self) -> int:
        """
:type: int"""
    @s.setter
    def s(self, arg0: int) -> None:
        ...

    @property
    def t(self) -> int:
        """
:type: int"""
    @t.setter
    def t(self, arg0: int) -> None:
        ...

    @property
    def x(self) -> int:
        """
:type: int"""
    @x.setter
    def x(self, arg0: int) -> None:
        ...

    @property
    def y(self) -> int:
        """
:type: int"""
    @y.setter
    def y(self, arg0: int) -> None:
        ...

    @property
    def z(self) -> int:
        """
:type: int"""
    @z.setter
    def z(self, arg0: int) -> None:
        ...

    pass

class u8vec4():
    @overload
    def __add__(self, arg0: u8vec4) -> u8vec4: 
        """__add__(*args, **kwargs)
Overloaded function.

1. __add__(self: ace.glm.u8vec4, arg0: ace.glm.u8vec4) -> ace.glm.u8vec4

2. __add__(self: ace.glm.u8vec4, arg0: int) -> ace.glm.u8vec4
"""

    @overload
    def __add__(self, arg0: int) -> u8vec4: ...

    @overload
    def __and__(self, arg0: int) -> u8vec4: 
        """__and__(*args, **kwargs)
Overloaded function.

1. __and__(self: ace.glm.u8vec4, arg0: ace.glm.u8vec4) -> ace.glm.u8vec4

2. __and__(self: ace.glm.u8vec4, arg0: int) -> ace.glm.u8vec4
"""

    @overload
    def __and__(self, arg0: u8vec4) -> u8vec4: ...

    def __eq__(self, arg0: u8vec4) -> bool: 
        """__eq__(self: ace.glm.u8vec4, arg0: ace.glm.u8vec4) -> bool
"""

    def __getitem__(self, arg0: int) -> int: 
        """__getitem__(self: ace.glm.u8vec4, arg0: int) -> int
"""

    def __hash__(self) -> int: 
        """__hash__(self: ace.glm.u8vec4) -> int
"""

    @overload
    def __iadd__(self, arg0: int) -> u8vec4: 
        """__iadd__(*args, **kwargs)
Overloaded function.

1. __iadd__(self: ace.glm.u8vec4, arg0: ace.glm.u8vec4) -> ace.glm.u8vec4

2. __iadd__(self: ace.glm.u8vec4, arg0: int) -> ace.glm.u8vec4
"""

    @overload
    def __iadd__(self, arg0: u8vec4) -> u8vec4: ...

    @overload
    def __iand__(self, arg0: int) -> u8vec4: 
        """__iand__(*args, **kwargs)
Overloaded function.

1. __iand__(self: ace.glm.u8vec4, arg0: ace.glm.u8vec4) -> ace.glm.u8vec4

2. __iand__(self: ace.glm.u8vec4, arg0: int) -> ace.glm.u8vec4
"""

    @overload
    def __iand__(self, arg0: u8vec4) -> u8vec4: ...

    @overload
    def __ilshift__(self, arg0: u8vec4) -> u8vec4: 
        """__ilshift__(*args, **kwargs)
Overloaded function.

1. __ilshift__(self: ace.glm.u8vec4, arg0: ace.glm.u8vec4) -> ace.glm.u8vec4

2. __ilshift__(self: ace.glm.u8vec4, arg0: int) -> ace.glm.u8vec4
"""

    @overload
    def __ilshift__(self, arg0: int) -> u8vec4: ...

    @overload
    def __imul__(self, arg0: u8vec4) -> u8vec4: 
        """__imul__(*args, **kwargs)
Overloaded function.

1. __imul__(self: ace.glm.u8vec4, arg0: ace.glm.u8vec4) -> ace.glm.u8vec4

2. __imul__(self: ace.glm.u8vec4, arg0: int) -> ace.glm.u8vec4
"""

    @overload
    def __imul__(self, arg0: int) -> u8vec4: ...

    @overload
    def __init__(self, arg0: int) -> None: 
        """__init__(*args, **kwargs)
Overloaded function.

1. __init__(self: ace.glm.u8vec4, x: int, y: int, z: int, w: int) -> None

2. __init__(self: ace.glm.u8vec4, arg0: int) -> None

3. __init__(self: ace.glm.u8vec4, arg0: ace.glm.u8vec4) -> None

4. __init__(self: ace.glm.u8vec4, arg0: ace.glm.vec2, arg1: int, arg2: int) -> None

5. __init__(self: ace.glm.u8vec4, arg0: ace.glm.vec3, arg1: int) -> None

6. __init__(self: ace.glm.u8vec4, arg0: ace.glm.vec4) -> None

7. __init__(self: ace.glm.u8vec4, arg0: ace.glm.ivec2, arg1: int, arg2: int) -> None

8. __init__(self: ace.glm.u8vec4, arg0: ace.glm.ivec3, arg1: int) -> None

9. __init__(self: ace.glm.u8vec4, arg0: ace.glm.ivec4) -> None

10. __init__(self: ace.glm.u8vec4, arg0: ace.glm.u8vec2, arg1: int, arg2: int) -> None

11. __init__(self: ace.glm.u8vec4, arg0: ace.glm.u8vec3, arg1: int) -> None
"""

    @overload
    def __init__(self, arg0: vec2, arg1: int, arg2: int) -> None: ...

    @overload
    def __init__(self, x: int, y: int, z: int, w: int) -> None: ...

    @overload
    def __init__(self, arg0: u8vec3, arg1: int) -> None: ...

    @overload
    def __init__(self, arg0: ivec2, arg1: int, arg2: int) -> None: ...

    @overload
    def __init__(self, arg0: vec3, arg1: int) -> None: ...

    @overload
    def __init__(self, arg0: vec4) -> None: ...

    @overload
    def __init__(self, arg0: ivec3, arg1: int) -> None: ...

    @overload
    def __init__(self, arg0: ivec4) -> None: ...

    @overload
    def __init__(self, arg0: u8vec2, arg1: int, arg2: int) -> None: ...

    @overload
    def __init__(self, arg0: u8vec4) -> None: ...

    @overload
    def __ior__(self, arg0: u8vec4) -> u8vec4: 
        """__ior__(*args, **kwargs)
Overloaded function.

1. __ior__(self: ace.glm.u8vec4, arg0: ace.glm.u8vec4) -> ace.glm.u8vec4

2. __ior__(self: ace.glm.u8vec4, arg0: int) -> ace.glm.u8vec4
"""

    @overload
    def __ior__(self, arg0: int) -> u8vec4: ...

    @overload
    def __irshift__(self, arg0: u8vec4) -> u8vec4: 
        """__irshift__(*args, **kwargs)
Overloaded function.

1. __irshift__(self: ace.glm.u8vec4, arg0: ace.glm.u8vec4) -> ace.glm.u8vec4

2. __irshift__(self: ace.glm.u8vec4, arg0: int) -> ace.glm.u8vec4
"""

    @overload
    def __irshift__(self, arg0: int) -> u8vec4: ...

    @overload
    def __isub__(self, arg0: u8vec4) -> u8vec4: 
        """__isub__(*args, **kwargs)
Overloaded function.

1. __isub__(self: ace.glm.u8vec4, arg0: ace.glm.u8vec4) -> ace.glm.u8vec4

2. __isub__(self: ace.glm.u8vec4, arg0: int) -> ace.glm.u8vec4
"""

    @overload
    def __isub__(self, arg0: int) -> u8vec4: ...

    @overload
    def __itruediv__(self, arg0: u8vec4) -> u8vec4: 
        """__itruediv__(*args, **kwargs)
Overloaded function.

1. __itruediv__(self: ace.glm.u8vec4, arg0: ace.glm.u8vec4) -> ace.glm.u8vec4

2. __itruediv__(self: ace.glm.u8vec4, arg0: int) -> ace.glm.u8vec4
"""

    @overload
    def __itruediv__(self, arg0: int) -> u8vec4: ...

    @overload
    def __ixor__(self, arg0: int) -> u8vec4: 
        """__ixor__(*args, **kwargs)
Overloaded function.

1. __ixor__(self: ace.glm.u8vec4, arg0: ace.glm.u8vec4) -> ace.glm.u8vec4

2. __ixor__(self: ace.glm.u8vec4, arg0: int) -> ace.glm.u8vec4
"""

    @overload
    def __ixor__(self, arg0: u8vec4) -> u8vec4: ...

    def __len__(self) -> int: 
        """__len__(self: ace.glm.u8vec4) -> int
"""

    @overload
    def __lshift__(self, arg0: int) -> u8vec4: 
        """__lshift__(*args, **kwargs)
Overloaded function.

1. __lshift__(self: ace.glm.u8vec4, arg0: ace.glm.u8vec4) -> ace.glm.u8vec4

2. __lshift__(self: ace.glm.u8vec4, arg0: int) -> ace.glm.u8vec4
"""

    @overload
    def __lshift__(self, arg0: u8vec4) -> u8vec4: ...

    @overload
    def __mul__(self, arg0: int) -> u8vec4: 
        """__mul__(*args, **kwargs)
Overloaded function.

1. __mul__(self: ace.glm.u8vec4, arg0: ace.glm.u8vec4) -> ace.glm.u8vec4

2. __mul__(self: ace.glm.u8vec4, arg0: int) -> ace.glm.u8vec4
"""

    @overload
    def __mul__(self, arg0: u8vec4) -> u8vec4: ...

    def __ne__(self, arg0: u8vec4) -> bool: 
        """__ne__(self: ace.glm.u8vec4, arg0: ace.glm.u8vec4) -> bool
"""

    @overload
    def __or__(self, arg0: u8vec4) -> u8vec4: 
        """__or__(*args, **kwargs)
Overloaded function.

1. __or__(self: ace.glm.u8vec4, arg0: ace.glm.u8vec4) -> ace.glm.u8vec4

2. __or__(self: ace.glm.u8vec4, arg0: int) -> ace.glm.u8vec4
"""

    @overload
    def __or__(self, arg0: int) -> u8vec4: ...

    def __radd__(self, arg0: int) -> u8vec4: 
        """__radd__(self: ace.glm.u8vec4, arg0: int) -> ace.glm.u8vec4
"""

    def __rand__(self, arg0: int) -> u8vec4: 
        """__rand__(self: ace.glm.u8vec4, arg0: int) -> ace.glm.u8vec4
"""

    def __repr__(self) -> str: 
        """__repr__(self: ace.glm.u8vec4) -> str
"""

    def __rlshift__(self, arg0: int) -> u8vec4: 
        """__rlshift__(self: ace.glm.u8vec4, arg0: int) -> ace.glm.u8vec4
"""

    def __rmul__(self, arg0: int) -> u8vec4: 
        """__rmul__(self: ace.glm.u8vec4, arg0: int) -> ace.glm.u8vec4
"""

    def __ror__(self, arg0: int) -> u8vec4: 
        """__ror__(self: ace.glm.u8vec4, arg0: int) -> ace.glm.u8vec4
"""

    def __rrshift__(self, arg0: int) -> u8vec4: 
        """__rrshift__(self: ace.glm.u8vec4, arg0: int) -> ace.glm.u8vec4
"""

    @overload
    def __rshift__(self, arg0: u8vec4) -> u8vec4: 
        """__rshift__(*args, **kwargs)
Overloaded function.

1. __rshift__(self: ace.glm.u8vec4, arg0: ace.glm.u8vec4) -> ace.glm.u8vec4

2. __rshift__(self: ace.glm.u8vec4, arg0: int) -> ace.glm.u8vec4
"""

    @overload
    def __rshift__(self, arg0: int) -> u8vec4: ...

    def __rsub__(self, arg0: int) -> u8vec4: 
        """__rsub__(self: ace.glm.u8vec4, arg0: int) -> ace.glm.u8vec4
"""

    def __rtruediv__(self, arg0: int) -> u8vec4: 
        """__rtruediv__(self: ace.glm.u8vec4, arg0: int) -> ace.glm.u8vec4
"""

    def __rxor__(self, arg0: int) -> u8vec4: 
        """__rxor__(self: ace.glm.u8vec4, arg0: int) -> ace.glm.u8vec4
"""

    def __setitem__(self, arg0: int, arg1: int) -> None: 
        """__setitem__(self: ace.glm.u8vec4, arg0: int, arg1: int) -> None
"""

    @overload
    def __sub__(self, arg0: u8vec4) -> u8vec4: 
        """__sub__(*args, **kwargs)
Overloaded function.

1. __sub__(self: ace.glm.u8vec4, arg0: ace.glm.u8vec4) -> ace.glm.u8vec4

2. __sub__(self: ace.glm.u8vec4, arg0: int) -> ace.glm.u8vec4
"""

    @overload
    def __sub__(self, arg0: int) -> u8vec4: ...

    @overload
    def __truediv__(self, arg0: u8vec4) -> u8vec4: 
        """__truediv__(*args, **kwargs)
Overloaded function.

1. __truediv__(self: ace.glm.u8vec4, arg0: ace.glm.u8vec4) -> ace.glm.u8vec4

2. __truediv__(self: ace.glm.u8vec4, arg0: int) -> ace.glm.u8vec4
"""

    @overload
    def __truediv__(self, arg0: int) -> u8vec4: ...

    @overload
    def __xor__(self, arg0: int) -> u8vec4: 
        """__xor__(*args, **kwargs)
Overloaded function.

1. __xor__(self: ace.glm.u8vec4, arg0: ace.glm.u8vec4) -> ace.glm.u8vec4

2. __xor__(self: ace.glm.u8vec4, arg0: int) -> ace.glm.u8vec4
"""

    @overload
    def __xor__(self, arg0: u8vec4) -> u8vec4: ...

    @property
    def a(self) -> int:
        """
:type: int"""
    @a.setter
    def a(self, arg0: int) -> None:
        ...

    @property
    def b(self) -> int:
        """
:type: int"""
    @b.setter
    def b(self, arg0: int) -> None:
        ...

    @property
    def g(self) -> int:
        """
:type: int"""
    @g.setter
    def g(self, arg0: int) -> None:
        ...

    @property
    def p(self) -> int:
        """
:type: int"""
    @p.setter
    def p(self, arg0: int) -> None:
        ...

    @property
    def q(self) -> int:
        """
:type: int"""
    @q.setter
    def q(self, arg0: int) -> None:
        ...

    @property
    def r(self) -> int:
        """
:type: int"""
    @r.setter
    def r(self, arg0: int) -> None:
        ...

    @property
    def s(self) -> int:
        """
:type: int"""
    @s.setter
    def s(self, arg0: int) -> None:
        ...

    @property
    def t(self) -> int:
        """
:type: int"""
    @t.setter
    def t(self, arg0: int) -> None:
        ...

    @property
    def w(self) -> int:
        """
:type: int"""
    @w.setter
    def w(self, arg0: int) -> None:
        ...

    @property
    def x(self) -> int:
        """
:type: int"""
    @x.setter
    def x(self, arg0: int) -> None:
        ...

    @property
    def y(self) -> int:
        """
:type: int"""
    @y.setter
    def y(self, arg0: int) -> None:
        ...

    @property
    def z(self) -> int:
        """
:type: int"""
    @z.setter
    def z(self, arg0: int) -> None:
        ...

    pass

class vec2():
    @overload
    def __add__(self, arg0: vec2) -> vec2: 
        """__add__(*args, **kwargs)
Overloaded function.

1. __add__(self: ace.glm.vec2, arg0: ace.glm.vec2) -> ace.glm.vec2

2. __add__(self: ace.glm.vec2, arg0: float) -> ace.glm.vec2
"""

    @overload
    def __add__(self, arg0: float) -> vec2: ...

    def __eq__(self, arg0: vec2) -> bool: 
        """__eq__(self: ace.glm.vec2, arg0: ace.glm.vec2) -> bool
"""

    def __getitem__(self, arg0: int) -> float: 
        """__getitem__(self: ace.glm.vec2, arg0: int) -> float
"""

    def __hash__(self) -> int: 
        """__hash__(self: ace.glm.vec2) -> int
"""

    @overload
    def __iadd__(self, arg0: float) -> vec2: 
        """__iadd__(*args, **kwargs)
Overloaded function.

1. __iadd__(self: ace.glm.vec2, arg0: ace.glm.vec2) -> ace.glm.vec2

2. __iadd__(self: ace.glm.vec2, arg0: float) -> ace.glm.vec2
"""

    @overload
    def __iadd__(self, arg0: vec2) -> vec2: ...

    @overload
    def __imul__(self, arg0: vec2) -> vec2: 
        """__imul__(*args, **kwargs)
Overloaded function.

1. __imul__(self: ace.glm.vec2, arg0: ace.glm.vec2) -> ace.glm.vec2

2. __imul__(self: ace.glm.vec2, arg0: float) -> ace.glm.vec2
"""

    @overload
    def __imul__(self, arg0: float) -> vec2: ...

    @overload
    def __init__(self, arg0: ivec2) -> None: 
        """__init__(*args, **kwargs)
Overloaded function.

1. __init__(self: ace.glm.vec2, x: float, y: float) -> None

2. __init__(self: ace.glm.vec2, x: float=0) -> None

3. __init__(self: ace.glm.vec2, arg0: ace.glm.vec2) -> None

4. __init__(self: ace.glm.vec2, arg0: ace.glm.vec3) -> None

5. __init__(self: ace.glm.vec2, arg0: ace.glm.vec4) -> None

6. __init__(self: ace.glm.vec2, arg0: ace.glm.ivec2) -> None

7. __init__(self: ace.glm.vec2, arg0: ace.glm.ivec3) -> None

8. __init__(self: ace.glm.vec2, arg0: ace.glm.ivec4) -> None

9. __init__(self: ace.glm.vec2, arg0: ace.glm.u8vec2) -> None

10. __init__(self: ace.glm.vec2, arg0: ace.glm.u8vec3) -> None

11. __init__(self: ace.glm.vec2, arg0: ace.glm.u8vec4) -> None

12. __init__(self: ace.glm.vec2, arg0: ace.glm.ivec2) -> None
"""

    @overload
    def __init__(self, arg0: vec2) -> None: ...

    @overload
    def __init__(self, arg0: vec4) -> None: ...

    @overload
    def __init__(self, arg0: u8vec3) -> None: ...

    @overload
    def __init__(self, arg0: u8vec2) -> None: ...

    @overload
    def __init__(self, arg0: ivec3) -> None: ...

    @overload
    def __init__(self, arg0: vec3) -> None: ...

    @overload
    def __init__(self, x: float, y: float) -> None: ...

    @overload
    def __init__(self, arg0: ivec4) -> None: ...

    @overload
    def __init__(self, arg0: u8vec4) -> None: ...

    @overload
    def __init__(self, x: float=0) -> None: ...

    @overload
    def __isub__(self, arg0: float) -> vec2: 
        """__isub__(*args, **kwargs)
Overloaded function.

1. __isub__(self: ace.glm.vec2, arg0: ace.glm.vec2) -> ace.glm.vec2

2. __isub__(self: ace.glm.vec2, arg0: float) -> ace.glm.vec2
"""

    @overload
    def __isub__(self, arg0: vec2) -> vec2: ...

    @overload
    def __itruediv__(self, arg0: float) -> vec2: 
        """__itruediv__(*args, **kwargs)
Overloaded function.

1. __itruediv__(self: ace.glm.vec2, arg0: ace.glm.vec2) -> ace.glm.vec2

2. __itruediv__(self: ace.glm.vec2, arg0: float) -> ace.glm.vec2
"""

    @overload
    def __itruediv__(self, arg0: vec2) -> vec2: ...

    def __len__(self) -> int: 
        """__len__(self: ace.glm.vec2) -> int
"""

    @overload
    def __mul__(self, arg0: vec2) -> vec2: 
        """__mul__(*args, **kwargs)
Overloaded function.

1. __mul__(self: ace.glm.vec2, arg0: ace.glm.vec2) -> ace.glm.vec2

2. __mul__(self: ace.glm.vec2, arg0: float) -> ace.glm.vec2
"""

    @overload
    def __mul__(self, arg0: float) -> vec2: ...

    def __ne__(self, arg0: vec2) -> bool: 
        """__ne__(self: ace.glm.vec2, arg0: ace.glm.vec2) -> bool
"""

    def __radd__(self, arg0: float) -> vec2: 
        """__radd__(self: ace.glm.vec2, arg0: float) -> ace.glm.vec2
"""

    def __repr__(self) -> str: 
        """__repr__(self: ace.glm.vec2) -> str
"""

    def __rmul__(self, arg0: float) -> vec2: 
        """__rmul__(self: ace.glm.vec2, arg0: float) -> ace.glm.vec2
"""

    def __rsub__(self, arg0: float) -> vec2: 
        """__rsub__(self: ace.glm.vec2, arg0: float) -> ace.glm.vec2
"""

    def __rtruediv__(self, arg0: float) -> vec2: 
        """__rtruediv__(self: ace.glm.vec2, arg0: float) -> ace.glm.vec2
"""

    def __setitem__(self, arg0: int, arg1: float) -> None: 
        """__setitem__(self: ace.glm.vec2, arg0: int, arg1: float) -> None
"""

    @overload
    def __sub__(self, arg0: float) -> vec2: 
        """__sub__(*args, **kwargs)
Overloaded function.

1. __sub__(self: ace.glm.vec2, arg0: ace.glm.vec2) -> ace.glm.vec2

2. __sub__(self: ace.glm.vec2, arg0: float) -> ace.glm.vec2
"""

    @overload
    def __sub__(self, arg0: vec2) -> vec2: ...

    @overload
    def __truediv__(self, arg0: float) -> vec2: 
        """__truediv__(*args, **kwargs)
Overloaded function.

1. __truediv__(self: ace.glm.vec2, arg0: ace.glm.vec2) -> ace.glm.vec2

2. __truediv__(self: ace.glm.vec2, arg0: float) -> ace.glm.vec2
"""

    @overload
    def __truediv__(self, arg0: vec2) -> vec2: ...

    @property
    def g(self) -> float:
        """
:type: float"""
    @g.setter
    def g(self, arg0: float) -> None:
        ...

    @property
    def r(self) -> float:
        """
:type: float"""
    @r.setter
    def r(self, arg0: float) -> None:
        ...

    @property
    def s(self) -> float:
        """
:type: float"""
    @s.setter
    def s(self, arg0: float) -> None:
        ...

    @property
    def t(self) -> float:
        """
:type: float"""
    @t.setter
    def t(self, arg0: float) -> None:
        ...

    @property
    def x(self) -> float:
        """
:type: float"""
    @x.setter
    def x(self, arg0: float) -> None:
        ...

    @property
    def y(self) -> float:
        """
:type: float"""
    @y.setter
    def y(self, arg0: float) -> None:
        ...

    pass

class vec3():
    @overload
    def __add__(self, arg0: vec3) -> vec3: 
        """__add__(*args, **kwargs)
Overloaded function.

1. __add__(self: ace.glm.vec3, arg0: ace.glm.vec3) -> ace.glm.vec3

2. __add__(self: ace.glm.vec3, arg0: float) -> ace.glm.vec3
"""

    @overload
    def __add__(self, arg0: float) -> vec3: ...

    def __eq__(self, arg0: vec3) -> bool: 
        """__eq__(self: ace.glm.vec3, arg0: ace.glm.vec3) -> bool
"""

    def __getitem__(self, arg0: int) -> float: 
        """__getitem__(self: ace.glm.vec3, arg0: int) -> float
"""

    def __hash__(self) -> int: 
        """__hash__(self: ace.glm.vec3) -> int
"""

    @overload
    def __iadd__(self, arg0: vec3) -> vec3: 
        """__iadd__(*args, **kwargs)
Overloaded function.

1. __iadd__(self: ace.glm.vec3, arg0: ace.glm.vec3) -> ace.glm.vec3

2. __iadd__(self: ace.glm.vec3, arg0: float) -> ace.glm.vec3
"""

    @overload
    def __iadd__(self, arg0: float) -> vec3: ...

    @overload
    def __imul__(self, arg0: float) -> vec3: 
        """__imul__(*args, **kwargs)
Overloaded function.

1. __imul__(self: ace.glm.vec3, arg0: ace.glm.vec3) -> ace.glm.vec3

2. __imul__(self: ace.glm.vec3, arg0: float) -> ace.glm.vec3
"""

    @overload
    def __imul__(self, arg0: vec3) -> vec3: ...

    @overload
    def __init__(self, arg0: u8vec3) -> None: 
        """__init__(*args, **kwargs)
Overloaded function.

1. __init__(self: ace.glm.vec3, x: float, y: float, z: float) -> None

2. __init__(self: ace.glm.vec3, x: float=0) -> None

3. __init__(self: ace.glm.vec3, arg0: ace.glm.vec3) -> None

4. __init__(self: ace.glm.vec3, arg0: ace.glm.vec2, arg1: float) -> None

5. __init__(self: ace.glm.vec3, arg0: ace.glm.vec4) -> None

6. __init__(self: ace.glm.vec3, arg0: ace.glm.ivec2, arg1: float) -> None

7. __init__(self: ace.glm.vec3, arg0: ace.glm.ivec3) -> None

8. __init__(self: ace.glm.vec3, arg0: ace.glm.ivec4) -> None

9. __init__(self: ace.glm.vec3, arg0: ace.glm.u8vec2, arg1: float) -> None

10. __init__(self: ace.glm.vec3, arg0: ace.glm.u8vec3) -> None

11. __init__(self: ace.glm.vec3, arg0: ace.glm.u8vec4) -> None

12. __init__(self: ace.glm.vec3, arg0: ace.glm.ivec3) -> None
"""

    @overload
    def __init__(self, arg0: u8vec4) -> None: ...

    @overload
    def __init__(self, arg0: vec4) -> None: ...

    @overload
    def __init__(self, arg0: vec2, arg1: float) -> None: ...

    @overload
    def __init__(self, x: float=0) -> None: ...

    @overload
    def __init__(self, arg0: vec3) -> None: ...

    @overload
    def __init__(self, arg0: u8vec2, arg1: float) -> None: ...

    @overload
    def __init__(self, x: float, y: float, z: float) -> None: ...

    @overload
    def __init__(self, arg0: ivec2, arg1: float) -> None: ...

    @overload
    def __init__(self, arg0: ivec4) -> None: ...

    @overload
    def __init__(self, arg0: ivec3) -> None: ...

    @overload
    def __isub__(self, arg0: float) -> vec3: 
        """__isub__(*args, **kwargs)
Overloaded function.

1. __isub__(self: ace.glm.vec3, arg0: ace.glm.vec3) -> ace.glm.vec3

2. __isub__(self: ace.glm.vec3, arg0: float) -> ace.glm.vec3
"""

    @overload
    def __isub__(self, arg0: vec3) -> vec3: ...

    @overload
    def __itruediv__(self, arg0: float) -> vec3: 
        """__itruediv__(*args, **kwargs)
Overloaded function.

1. __itruediv__(self: ace.glm.vec3, arg0: ace.glm.vec3) -> ace.glm.vec3

2. __itruediv__(self: ace.glm.vec3, arg0: float) -> ace.glm.vec3
"""

    @overload
    def __itruediv__(self, arg0: vec3) -> vec3: ...

    def __len__(self) -> int: 
        """__len__(self: ace.glm.vec3) -> int
"""

    @overload
    def __mul__(self, arg0: float) -> vec3: 
        """__mul__(*args, **kwargs)
Overloaded function.

1. __mul__(self: ace.glm.vec3, arg0: ace.glm.vec3) -> ace.glm.vec3

2. __mul__(self: ace.glm.vec3, arg0: float) -> ace.glm.vec3
"""

    @overload
    def __mul__(self, arg0: vec3) -> vec3: ...

    def __ne__(self, arg0: vec3) -> bool: 
        """__ne__(self: ace.glm.vec3, arg0: ace.glm.vec3) -> bool
"""

    def __radd__(self, arg0: float) -> vec3: 
        """__radd__(self: ace.glm.vec3, arg0: float) -> ace.glm.vec3
"""

    def __repr__(self) -> str: 
        """__repr__(self: ace.glm.vec3) -> str
"""

    def __rmul__(self, arg0: float) -> vec3: 
        """__rmul__(self: ace.glm.vec3, arg0: float) -> ace.glm.vec3
"""

    def __rsub__(self, arg0: float) -> vec3: 
        """__rsub__(self: ace.glm.vec3, arg0: float) -> ace.glm.vec3
"""

    def __rtruediv__(self, arg0: float) -> vec3: 
        """__rtruediv__(self: ace.glm.vec3, arg0: float) -> ace.glm.vec3
"""

    def __setitem__(self, arg0: int, arg1: float) -> None: 
        """__setitem__(self: ace.glm.vec3, arg0: int, arg1: float) -> None
"""

    @overload
    def __sub__(self, arg0: vec3) -> vec3: 
        """__sub__(*args, **kwargs)
Overloaded function.

1. __sub__(self: ace.glm.vec3, arg0: ace.glm.vec3) -> ace.glm.vec3

2. __sub__(self: ace.glm.vec3, arg0: float) -> ace.glm.vec3
"""

    @overload
    def __sub__(self, arg0: float) -> vec3: ...

    @overload
    def __truediv__(self, arg0: float) -> vec3: 
        """__truediv__(*args, **kwargs)
Overloaded function.

1. __truediv__(self: ace.glm.vec3, arg0: ace.glm.vec3) -> ace.glm.vec3

2. __truediv__(self: ace.glm.vec3, arg0: float) -> ace.glm.vec3
"""

    @overload
    def __truediv__(self, arg0: vec3) -> vec3: ...

    @property
    def b(self) -> float:
        """
:type: float"""
    @b.setter
    def b(self, arg0: float) -> None:
        ...

    @property
    def g(self) -> float:
        """
:type: float"""
    @g.setter
    def g(self, arg0: float) -> None:
        ...

    @property
    def p(self) -> float:
        """
:type: float"""
    @p.setter
    def p(self, arg0: float) -> None:
        ...

    @property
    def r(self) -> float:
        """
:type: float"""
    @r.setter
    def r(self, arg0: float) -> None:
        ...

    @property
    def s(self) -> float:
        """
:type: float"""
    @s.setter
    def s(self, arg0: float) -> None:
        ...

    @property
    def t(self) -> float:
        """
:type: float"""
    @t.setter
    def t(self, arg0: float) -> None:
        ...

    @property
    def x(self) -> float:
        """
:type: float"""
    @x.setter
    def x(self, arg0: float) -> None:
        ...

    @property
    def y(self) -> float:
        """
:type: float"""
    @y.setter
    def y(self, arg0: float) -> None:
        ...

    @property
    def z(self) -> float:
        """
:type: float"""
    @z.setter
    def z(self, arg0: float) -> None:
        ...

    pass

class vec4():
    @overload
    def __add__(self, arg0: float) -> vec4: 
        """__add__(*args, **kwargs)
Overloaded function.

1. __add__(self: ace.glm.vec4, arg0: ace.glm.vec4) -> ace.glm.vec4

2. __add__(self: ace.glm.vec4, arg0: float) -> ace.glm.vec4
"""

    @overload
    def __add__(self, arg0: vec4) -> vec4: ...

    def __eq__(self, arg0: vec4) -> bool: 
        """__eq__(self: ace.glm.vec4, arg0: ace.glm.vec4) -> bool
"""

    def __getitem__(self, arg0: int) -> float: 
        """__getitem__(self: ace.glm.vec4, arg0: int) -> float
"""

    def __hash__(self) -> int: 
        """__hash__(self: ace.glm.vec4) -> int
"""

    @overload
    def __iadd__(self, arg0: vec4) -> vec4: 
        """__iadd__(*args, **kwargs)
Overloaded function.

1. __iadd__(self: ace.glm.vec4, arg0: ace.glm.vec4) -> ace.glm.vec4

2. __iadd__(self: ace.glm.vec4, arg0: float) -> ace.glm.vec4
"""

    @overload
    def __iadd__(self, arg0: float) -> vec4: ...

    @overload
    def __imul__(self, arg0: vec4) -> vec4: 
        """__imul__(*args, **kwargs)
Overloaded function.

1. __imul__(self: ace.glm.vec4, arg0: ace.glm.vec4) -> ace.glm.vec4

2. __imul__(self: ace.glm.vec4, arg0: float) -> ace.glm.vec4
"""

    @overload
    def __imul__(self, arg0: float) -> vec4: ...

    @overload
    def __init__(self, arg0: u8vec3, arg1: float) -> None: 
        """__init__(*args, **kwargs)
Overloaded function.

1. __init__(self: ace.glm.vec4, x: float, y: float, z: float, w: float) -> None

2. __init__(self: ace.glm.vec4, arg0: float) -> None

3. __init__(self: ace.glm.vec4, arg0: ace.glm.vec4) -> None

4. __init__(self: ace.glm.vec4, arg0: ace.glm.vec2, arg1: float, arg2: float) -> None

5. __init__(self: ace.glm.vec4, arg0: ace.glm.vec3, arg1: float) -> None

6. __init__(self: ace.glm.vec4, arg0: ace.glm.ivec2, arg1: float, arg2: float) -> None

7. __init__(self: ace.glm.vec4, arg0: ace.glm.ivec3, arg1: float) -> None

8. __init__(self: ace.glm.vec4, arg0: ace.glm.ivec4) -> None

9. __init__(self: ace.glm.vec4, arg0: ace.glm.u8vec2, arg1: float, arg2: float) -> None

10. __init__(self: ace.glm.vec4, arg0: ace.glm.u8vec3, arg1: float) -> None

11. __init__(self: ace.glm.vec4, arg0: ace.glm.u8vec4) -> None

12. __init__(self: ace.glm.vec4, arg0: ace.glm.ivec4) -> None
"""

    @overload
    def __init__(self, arg0: float) -> None: ...

    @overload
    def __init__(self, arg0: u8vec4) -> None: ...

    @overload
    def __init__(self, arg0: u8vec2, arg1: float, arg2: float) -> None: ...

    @overload
    def __init__(self, arg0: ivec2, arg1: float, arg2: float) -> None: ...

    @overload
    def __init__(self, arg0: ivec3, arg1: float) -> None: ...

    @overload
    def __init__(self, arg0: vec2, arg1: float, arg2: float) -> None: ...

    @overload
    def __init__(self, arg0: vec3, arg1: float) -> None: ...

    @overload
    def __init__(self, arg0: vec4) -> None: ...

    @overload
    def __init__(self, x: float, y: float, z: float, w: float) -> None: ...

    @overload
    def __init__(self, arg0: ivec4) -> None: ...

    @overload
    def __isub__(self, arg0: vec4) -> vec4: 
        """__isub__(*args, **kwargs)
Overloaded function.

1. __isub__(self: ace.glm.vec4, arg0: ace.glm.vec4) -> ace.glm.vec4

2. __isub__(self: ace.glm.vec4, arg0: float) -> ace.glm.vec4
"""

    @overload
    def __isub__(self, arg0: float) -> vec4: ...

    @overload
    def __itruediv__(self, arg0: vec4) -> vec4: 
        """__itruediv__(*args, **kwargs)
Overloaded function.

1. __itruediv__(self: ace.glm.vec4, arg0: ace.glm.vec4) -> ace.glm.vec4

2. __itruediv__(self: ace.glm.vec4, arg0: float) -> ace.glm.vec4
"""

    @overload
    def __itruediv__(self, arg0: float) -> vec4: ...

    def __len__(self) -> int:
        """__len__(self: ace.glm.vec4) -> int
"""

    @overload
    def __mul__(self, arg0: float) -> vec4: 
        """__mul__(*args, **kwargs)
Overloaded function.

1. __mul__(self: ace.glm.vec4, arg0: ace.glm.vec4) -> ace.glm.vec4

2. __mul__(self: ace.glm.vec4, arg0: float) -> ace.glm.vec4
"""

    @overload
    def __mul__(self, arg0: vec4) -> vec4: ...

    def __ne__(self, arg0: vec4) -> bool: 
        """__ne__(self: ace.glm.vec4, arg0: ace.glm.vec4) -> bool
"""

    def __radd__(self, arg0: float) -> vec4: 
        """__radd__(self: ace.glm.vec4, arg0: float) -> ace.glm.vec4
"""

    def __repr__(self) -> str: 
        """__repr__(self: ace.glm.vec4) -> str
"""

    def __rmul__(self, arg0: float) -> vec4: 
        """__rmul__(self: ace.glm.vec4, arg0: float) -> ace.glm.vec4
"""

    def __rsub__(self, arg0: float) -> vec4: 
        """__rsub__(self: ace.glm.vec4, arg0: float) -> ace.glm.vec4
"""

    def __rtruediv__(self, arg0: float) -> vec4: 
        """__rtruediv__(self: ace.glm.vec4, arg0: float) -> ace.glm.vec4
"""

    def __setitem__(self, arg0: int, arg1: float) -> None: 
        """__setitem__(self: ace.glm.vec4, arg0: int, arg1: float) -> None
"""

    @overload
    def __sub__(self, arg0: float) -> vec4: 
        """__sub__(*args, **kwargs)
Overloaded function.

1. __sub__(self: ace.glm.vec4, arg0: ace.glm.vec4) -> ace.glm.vec4

2. __sub__(self: ace.glm.vec4, arg0: float) -> ace.glm.vec4
"""

    @overload
    def __sub__(self, arg0: vec4) -> vec4: ...

    @overload
    def __truediv__(self, arg0: vec4) -> vec4: 
        """__truediv__(*args, **kwargs)
Overloaded function.

1. __truediv__(self: ace.glm.vec4, arg0: ace.glm.vec4) -> ace.glm.vec4

2. __truediv__(self: ace.glm.vec4, arg0: float) -> ace.glm.vec4
"""

    @overload
    def __truediv__(self, arg0: float) -> vec4: ...

    @property
    def a(self) -> float:
        """
:type: float"""
    @a.setter
    def a(self, arg0: float) -> None:
        ...

    @property
    def b(self) -> float:
        """
:type: float"""
    @b.setter
    def b(self, arg0: float) -> None:
        ...

    @property
    def g(self) -> float:
        """
:type: float"""
    @g.setter
    def g(self, arg0: float) -> None:
        ...

    @property
    def p(self) -> float:
        """
:type: float"""
    @p.setter
    def p(self, arg0: float) -> None:
        ...

    @property
    def q(self) -> float:
        """
:type: float"""
    @q.setter
    def q(self, arg0: float) -> None:
        ...

    @property
    def r(self) -> float:
        """
:type: float"""
    @r.setter
    def r(self, arg0: float) -> None:
        ...

    @property
    def s(self) -> float:
        """
:type: float"""
    @s.setter
    def s(self, arg0: float) -> None:
        ...

    @property
    def t(self) -> float:
        """
:type: float"""
    @t.setter
    def t(self, arg0: float) -> None:
        ...

    @property
    def w(self) -> float:
        """
:type: float"""
    @w.setter
    def w(self, arg0: float) -> None:
        ...

    @property
    def x(self) -> float:
        """
:type: float"""
    @x.setter
    def x(self, arg0: float) -> None:
        ...

    @property
    def y(self) -> float:
        """
:type: float"""
    @y.setter
    def y(self, arg0: float) -> None:
        ...

    @property
    def z(self) -> float:
        """
:type: float"""
    @z.setter
    def z(self, arg0: float) -> None:
        ...

    pass

@overload
def distance(x: vec4, y: vec4) -> float:
    """distance(*args, **kwargs)
Overloaded function.

1. distance(x: ace.glm.vec2, y: ace.glm.vec2) -> float

Returns the distance between x and y, i.e., length(x - y).

2. distance(x: ace.glm.vec3, y: ace.glm.vec3) -> float

Returns the distance between x and y, i.e., length(x - y).

3. distance(x: ace.glm.vec4, y: ace.glm.vec4) -> float

Returns the distance between x and y, i.e., length(x - y).
"""
@overload
def distance(x: vec2, y: vec2) -> float:
    pass
@overload
def distance(x: vec3, y: vec3) -> float:
    pass
@overload
def dot(x: vec2, y: vec2) -> float:
    """dot(*args, **kwargs)
Overloaded function.

1. dot(x: ace.glm.vec2, y: ace.glm.vec2) -> float

Returns the dot product of x and y, i.e., result = x * y.

2. dot(x: ace.glm.vec3, y: ace.glm.vec3) -> float

Returns the dot product of x and y, i.e., result = x * y.

3. dot(x: ace.glm.vec4, y: ace.glm.vec4) -> float

Returns the dot product of x and y, i.e., result = x * y.
"""
@overload
def dot(x: vec4, y: vec4) -> float:
    pass
@overload
def dot(x: vec3, y: vec3) -> float:
    pass
@overload
def length(x: vec3) -> float:
    """length(*args, **kwargs)
Overloaded function.

1. length(x: ace.glm.vec2) -> float

Returns the length of x, i.e., sqrt(x * x).

2. length(x: ace.glm.vec3) -> float

Returns the length of x, i.e., sqrt(x * x).

3. length(x: ace.glm.vec4) -> float

Returns the length of x, i.e., sqrt(x * x).
"""
@overload
def length(x: vec2) -> float:
    pass
@overload
def length(x: vec4) -> float:
    pass
@overload
def max(x: u8vec3, y: int) -> u8vec3:
    """max(*args, **kwargs)
Overloaded function.

1. max(x: ace.glm.vec2, y: ace.glm.vec2) -> ace.glm.vec2

Returns y if x < y; otherwise, it returns x.

2. max(x: ace.glm.vec2, y: float) -> ace.glm.vec2

Returns y if x < y; otherwise, it returns x.

3. max(x: ace.glm.vec3, y: ace.glm.vec3) -> ace.glm.vec3

Returns y if x < y; otherwise, it returns x.

4. max(x: ace.glm.vec3, y: float) -> ace.glm.vec3

Returns y if x < y; otherwise, it returns x.

5. max(x: ace.glm.vec4, y: ace.glm.vec4) -> ace.glm.vec4

Returns y if x < y; otherwise, it returns x.

6. max(x: ace.glm.vec4, y: float) -> ace.glm.vec4

Returns y if x < y; otherwise, it returns x.

7. max(x: ace.glm.ivec2, y: ace.glm.ivec2) -> ace.glm.ivec2

Returns y if x < y; otherwise, it returns x.

8. max(x: ace.glm.ivec2, y: int) -> ace.glm.ivec2

Returns y if x < y; otherwise, it returns x.

9. max(x: ace.glm.ivec3, y: ace.glm.ivec3) -> ace.glm.ivec3

Returns y if x < y; otherwise, it returns x.

10. max(x: ace.glm.ivec3, y: int) -> ace.glm.ivec3

Returns y if x < y; otherwise, it returns x.

11. max(x: ace.glm.ivec4, y: ace.glm.ivec4) -> ace.glm.ivec4

Returns y if x < y; otherwise, it returns x.

12. max(x: ace.glm.ivec4, y: int) -> ace.glm.ivec4

Returns y if x < y; otherwise, it returns x.

13. max(x: ace.glm.u8vec2, y: ace.glm.u8vec2) -> ace.glm.u8vec2

Returns y if x < y; otherwise, it returns x.

14. max(x: ace.glm.u8vec2, y: int) -> ace.glm.u8vec2

Returns y if x < y; otherwise, it returns x.

15. max(x: ace.glm.u8vec3, y: ace.glm.u8vec3) -> ace.glm.u8vec3

Returns y if x < y; otherwise, it returns x.

16. max(x: ace.glm.u8vec3, y: int) -> ace.glm.u8vec3

Returns y if x < y; otherwise, it returns x.

17. max(x: ace.glm.u8vec4, y: ace.glm.u8vec4) -> ace.glm.u8vec4

Returns y if x < y; otherwise, it returns x.

18. max(x: ace.glm.u8vec4, y: int) -> ace.glm.u8vec4

Returns y if x < y; otherwise, it returns x.
"""
@overload
def max(x: ivec3, y: int) -> ivec3:
    pass
@overload
def max(x: u8vec2, y: u8vec2) -> u8vec2:
    pass
@overload
def max(x: u8vec2, y: int) -> u8vec2:
    pass
@overload
def max(x: ivec4, y: ivec4) -> ivec4:
    pass
@overload
def max(x: vec4, y: vec4) -> vec4:
    pass
@overload
def max(x: ivec3, y: ivec3) -> ivec3:
    pass
@overload
def max(x: ivec4, y: int) -> ivec4:
    pass
@overload
def max(x: u8vec4, y: u8vec4) -> u8vec4:
    pass
@overload
def max(x: u8vec4, y: int) -> u8vec4:
    pass
@overload
def max(x: vec3, y: float) -> vec3:
    pass
@overload
def max(x: vec2, y: vec2) -> vec2:
    pass
@overload
def max(x: vec4, y: float) -> vec4:
    pass
@overload
def max(x: ivec2, y: ivec2) -> ivec2:
    pass
@overload
def max(x: vec3, y: vec3) -> vec3:
    pass
@overload
def max(x: u8vec3, y: u8vec3) -> u8vec3:
    pass
@overload
def max(x: vec2, y: float) -> vec2:
    pass
@overload
def max(x: ivec2, y: int) -> ivec2:
    pass
@overload
def min(x: ivec3, y: ivec3) -> ivec3:
    """min(*args, **kwargs)
Overloaded function.

1. min(x: ace.glm.vec2, y: ace.glm.vec2) -> ace.glm.vec2

Returns y if x < y; otherwise, it returns x.

2. min(x: ace.glm.vec2, y: float) -> ace.glm.vec2

Returns y if x < y; otherwise, it returns x.

3. min(x: ace.glm.vec3, y: ace.glm.vec3) -> ace.glm.vec3

Returns y if x < y; otherwise, it returns x.

4. min(x: ace.glm.vec3, y: float) -> ace.glm.vec3

Returns y if x < y; otherwise, it returns x.

5. min(x: ace.glm.vec4, y: ace.glm.vec4) -> ace.glm.vec4

Returns y if x < y; otherwise, it returns x.

6. min(x: ace.glm.vec4, y: float) -> ace.glm.vec4

Returns y if x < y; otherwise, it returns x.

7. min(x: ace.glm.ivec2, y: ace.glm.ivec2) -> ace.glm.ivec2

Returns y if x < y; otherwise, it returns x.

8. min(x: ace.glm.ivec2, y: int) -> ace.glm.ivec2

Returns y if x < y; otherwise, it returns x.

9. min(x: ace.glm.ivec3, y: ace.glm.ivec3) -> ace.glm.ivec3

Returns y if x < y; otherwise, it returns x.

10. min(x: ace.glm.ivec3, y: int) -> ace.glm.ivec3

Returns y if x < y; otherwise, it returns x.

11. min(x: ace.glm.ivec4, y: ace.glm.ivec4) -> ace.glm.ivec4

Returns y if x < y; otherwise, it returns x.

12. min(x: ace.glm.ivec4, y: int) -> ace.glm.ivec4

Returns y if x < y; otherwise, it returns x.

13. min(x: ace.glm.u8vec2, y: ace.glm.u8vec2) -> ace.glm.u8vec2

Returns y if x < y; otherwise, it returns x.

14. min(x: ace.glm.u8vec2, y: int) -> ace.glm.u8vec2

Returns y if x < y; otherwise, it returns x.

15. min(x: ace.glm.u8vec3, y: ace.glm.u8vec3) -> ace.glm.u8vec3

Returns y if x < y; otherwise, it returns x.

16. min(x: ace.glm.u8vec3, y: int) -> ace.glm.u8vec3

Returns y if x < y; otherwise, it returns x.

17. min(x: ace.glm.u8vec4, y: ace.glm.u8vec4) -> ace.glm.u8vec4

Returns y if x < y; otherwise, it returns x.

18. min(x: ace.glm.u8vec4, y: int) -> ace.glm.u8vec4

Returns y if x < y; otherwise, it returns x.
"""
@overload
def min(x: ivec4, y: int) -> ivec4:
    pass
@overload
def min(x: vec3, y: vec3) -> vec3:
    pass
@overload
def min(x: ivec2, y: int) -> ivec2:
    pass
@overload
def min(x: vec3, y: float) -> vec3:
    pass
@overload
def min(x: vec2, y: vec2) -> vec2:
    pass
@overload
def min(x: vec4, y: vec4) -> vec4:
    pass
@overload
def min(x: vec4, y: float) -> vec4:
    pass
@overload
def min(x: ivec2, y: ivec2) -> ivec2:
    pass
@overload
def min(x: u8vec3, y: u8vec3) -> u8vec3:
    pass
@overload
def min(x: vec2, y: float) -> vec2:
    pass
@overload
def min(x: u8vec3, y: int) -> u8vec3:
    pass
@overload
def min(x: u8vec2, y: int) -> u8vec2:
    pass
@overload
def min(x: u8vec4, y: u8vec4) -> u8vec4:
    pass
@overload
def min(x: u8vec2, y: u8vec2) -> u8vec2:
    pass
@overload
def min(x: ivec3, y: int) -> ivec3:
    pass
@overload
def min(x: u8vec4, y: int) -> u8vec4:
    pass
@overload
def min(x: ivec4, y: ivec4) -> ivec4:
    pass
@overload
def normalize(x: vec4) -> vec4:
    """normalize(*args, **kwargs)
Overloaded function.

1. normalize(x: ace.glm.vec2) -> ace.glm.vec2

Returns a vector in the same direction as x but with length of 1.

2. normalize(x: ace.glm.vec3) -> ace.glm.vec3

Returns a vector in the same direction as x but with length of 1.

3. normalize(x: ace.glm.vec4) -> ace.glm.vec4

Returns a vector in the same direction as x but with length of 1.
"""
@overload
def normalize(x: vec2) -> vec2:
    pass
@overload
def normalize(x: vec3) -> vec3:
    pass
