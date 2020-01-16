import time
from enum import Enum
from typing import Optional

from ace import glm, map, draw, Button, Key, Keymod, gl
from ace.gl import ShaderProgram
from ace.glm import ivec3, vec3
from ace.editor import kv6_to_blocks, BoxSelection, LineSelection, FaceSelection

from . import scene

# yeah i know
cost = {0: 1, 90: 0, 180: -1, 270: 0}
sint = {0: 0, 90: 1, 180: 0, 270: -1}


class BuildMode(Enum):
    BUILD = "Build"
    DELETE = "Delete"
    PAINT = "Paint"


class Tool:
    name = "BaseTool"

    def __init__(self, parent: 'scene.EditorScript'):
        self.parent = parent

    def update(self, dt: float):
        pass

    def draw(self):
        pass

    def on_mouse_button(self, b: Button, pressed: bool):
        pass

    def on_mouse_scroll(self, vertical: int, horizontal: int):
        pass

    def on_key(self, scancode: Key, modifiers: Keymod, pressed: bool):
        pass


class FaceTool(Tool):
    name = "Face"

    def __init__(self, parent: 'scene.EditorScript'):
        super().__init__(parent)

        self.selection = FaceSelection(self.parent.editor.map)
        self.depth = 1

    def on_mouse_scroll(self, vertical: int, horizontal: int):
        self.depth = max(1, self.depth + vertical)
        print(self.depth)

    def on_mouse_button(self, b: Button, pressed: bool):
        if not pressed: return

        f, hit = self.parent.editor.hitcast()

        t1 = time.perf_counter()
        self.selection.set(hit, f)
        t2 = time.perf_counter()
        print(f"Time to get adjacent faces: {t2 - t1}s")

        t1 = time.perf_counter()
        if self.parent.mode == BuildMode.BUILD:
            self.selection.build(self.parent.color, f, self.depth)
        elif self.parent.mode == BuildMode.DELETE:
            self.selection.destroy()
        elif self.parent.mode == BuildMode.PAINT:
            self.selection.paint(self.parent.color)
        t2 = time.perf_counter()
        print(f"Time to act on faces: {t2 - t1}s")


class ModelTool(Tool):
    name = "Model"

    def __init__(self, parent: 'scene.EditorScript', model: str="cp.kv6"):
        super().__init__(parent)

        if not model.endswith(".kv6"): model += ".kv6"
        self.name = f"Model ({model})"

        self.blocks = kv6_to_blocks(f"kv6/{model}")
        self.max_height = max(self.blocks.keys(), key=lambda b: b.z).z
        self.outline = draw.VXLBlocks(self.blocks, vec3(0, 0, self.max_height))
        self.outline_offset = vec3(0, 0, 0) # for rotation
        self.s: ShaderProgram = self.parent.editor.client.shader("map")

    def update(self, dt: float):
        f, hit = self.parent.editor.hitcast()
        hit = map.next_block(hit.x, hit.y, hit.z, f)
        self.outline.position = map.vox2draw(hit) + self.outline_offset

    def draw(self):
        self.s.bind()
        self.s.uniform("alpha", 0.8)
        self.s.uniform("replacement_color", vec3(0, 0, 0))
        self.s.uniform("filter_color", vec3(0, 0, 0))

        gl.Blend(True)
        self.outline.draw(self.s)
        gl.Blend(False)

    def on_mouse_button(self, b: Button, pressed: bool):
        if not pressed: return

        f, hit = self.parent.editor.hitcast()
        hit = map.next_block(hit.x, hit.y, hit.z, f)

        blocks = []
        for pos, color in self.blocks.items():
            if color.r == 0 and color.g == 0 and color.b == 0:
                color = self.parent.color

            rotation = int(self.outline.rotation.y)

            # yeah uhh high school geometry represent
            x = int(pos.x)
            y = int(pos.y)
            rx = x * cost[rotation] + y * sint[rotation]
            ry = y * cost[rotation] - x * sint[rotation]

            blocks.append((glm.ivec3(rx + hit.x, ry + hit.y, int(pos.z + hit.z - self.max_height)), color))
        self.parent.editor.map.set_blocks(blocks)

    def on_key(self, scancode: Key, modifiers: Keymod, pressed: bool):
        if not pressed: return

        if scancode == Key.UP:
            self.outline.rotation.y = 0
            self.outline_offset = vec3(0, 0, 0)
        elif scancode == Key.LEFT:
            self.outline.rotation.y = 90
            self.outline_offset = vec3(0, 0, 1)
        elif scancode == Key.DOWN:
            self.outline.rotation.y = 180
            self.outline_offset = vec3(1, 0, 1)
        elif scancode == Key.RIGHT:
            self.outline.rotation.y = 270
            self.outline_offset = vec3(1, 0, 0)


class CornerTool(Tool):
    name = "BaseCornerTool"

    def __init__(self, parent: 'scene.EditorScript'):
        super().__init__(parent)

        self.start = ivec3(-1)
        self.end = ivec3(-1)
        self.select_button: Optional[Button] = None

    def update(self, dt: float):
        if map.is_valid_pos(self.start):
            f, hit = self.parent.editor.hitcast()
            if self.parent.mode == BuildMode.BUILD:
                hit = map.next_block(hit.x, hit.y, hit.z, f)

            if self.end != hit and map.is_valid_pos(hit):
                self.end = hit
                self._selection_changed(self.start, self.end, self.select_button)

    def on_mouse_button(self, b: Button, pressed: bool):
        f, hit = self.parent.editor.hitcast()
        if f == map.Face.INVALID: return

        if self.parent.mode == BuildMode.BUILD:
            hit = map.next_block(hit.x, hit.y, hit.z, f)

        if not map.is_valid_pos(hit): return

        if pressed:
            if self.is_selecting(): return self._cancel()

            self.start = hit
            self.select_button = b
            self._selection_start(self.start, self.select_button)
        else:
            if not map.is_valid_pos(self.start): return
            self._selection_done(self.start, hit, self.select_button)
            self.start = ivec3(-1)
            self.end = ivec3(-1)

    def _selection_start(self, start: ivec3, mode: Button):
        pass

    def _selection_changed(self, start: ivec3, end: ivec3, mode: Button):
        pass

    def _selection_done(self, start: ivec3, end: ivec3, mode: Button):
        pass

    def _cancel(self):
        self.start = ivec3(-1)
        self.end = ivec3(-1)
        self.select_button = None

    def is_selecting(self):
        return map.is_valid_pos(self.start) and map.is_valid_pos(self.end)


class BlockLineTool(CornerTool):
    name = "Line"

    def __init__(self, parent: 'scene.EditorScript'):
        super().__init__(parent)
        self.outline = draw.VXLBlocks(glm.u8vec3(0))
        self.s: ShaderProgram = self.parent.editor.client.shader("map")

        self.selection = LineSelection(self.parent.editor.map)

    def draw(self):
        if not self.is_selecting(): return

        self.s.bind()
        self.s.uniform("alpha", 0.6)
        self.s.uniform("replacement_color", vec3(0, 0, 0))

        gl.Blend(True)
        gl.DepthTest(False)
        self.outline.draw(self.s)
        gl.Blend(False)
        gl.DepthTest(True)

    def _selection_changed(self, start: ivec3, end: ivec3, mode: Button):
        self.update_outline(start, end)

    def _selection_done(self, start: ivec3, end: ivec3, mode: Button):
        self.update_outline(start, end, mode == Button.LEFT)

    def update_outline(self, start: ivec3, end: ivec3, build: bool=False):
        self.selection.set(start, end)

        if build:
            if self.parent.mode == BuildMode.BUILD:
                self.selection.build(self.parent.color)
            elif self.parent.mode == BuildMode.DELETE:
                self.selection.destroy()
            elif self.parent.mode == BuildMode.PAINT:
                self.selection.paint(self.parent.color)

        self.outline.update({pos: self.parent.color for pos in self.selection.get_blocks()}, end)
        self.outline.position = map.vox2draw(end)


class BlockBoxTool(CornerTool):
    name = "Box"

    def __init__(self, parent: 'scene.EditorScript'):
        super().__init__(parent)
        self.center = vec3(0)
        self.size = vec3(0)
        self.high = None
        self.low = None

        self.selection = BoxSelection(self.parent.editor.map)

    def draw(self):
        if not self.is_selecting(): return

        self.parent.editor.debug.draw_cube(self.center, self.size, vec3(1, 0, 0))

    def _selection_changed(self, start: ivec3, end: ivec3, mode: Button):
        self.low = glm.min(start, end)
        self.high = glm.max(start, end)
        self.selection.set(self.low, self.high)
        size = vec3(self.high - self.low)

        self.center = map.vox2draw(vec3(self.low) + (size / 2) + 0.5)
        self.size = map.vox2draw(size + 1)

    def _selection_done(self, start: ivec3, end: ivec3, mode: Button):
        if mode != Button.LEFT: return False

        self.low = glm.min(start, end)
        self.high = glm.max(start, end)
        self.selection.set(self.low, self.high)

        if self.parent.mode == BuildMode.BUILD:
            self.selection.build(self.parent.color)
        elif self.parent.mode == BuildMode.DELETE:
            self.selection.destroy()
        elif self.parent.mode == BuildMode.PAINT:
            self.selection.paint(self.parent.color)

        return True
