# import asyncio
import itertools

from ace import glm, font, map, draw, Button, Key, Keymod
from ace.font import Align
from ace.glm import vec2, vec3, u8vec3
from ace.editor import MapEditor
import random

from . import tools
from loader import Script
import commands


def random_color():
    return u8vec3(random.randrange(0, 255), random.randrange(0, 255), random.randrange(0, 255))


class EditorScript(Script):
    def __init__(self, editor: MapEditor):
        super().__init__(editor)

        # self.loop: asyncio.AbstractEventLoop = asyncio.get_event_loop()

        self.font: font.Font = editor.fonts("Vera.ttf", 32)
        self.tool: tools.Tool = tools.FaceTool(self)
        self.color = random_color()

        self.mode_cycle = itertools.cycle(tools.BuildMode)
        self.mode: tools.BuildMode = next(self.mode_cycle)

        self.command_manager = commands.CommandManager()
        self.commands = BaseCommands(self)
        self.command_manager.add_commands(self.commands)
        self.tasks = []

        self.msg = ""
        self.msg_time = 0.0

        print("Loaded!")

    def deinit(self):
        print("Unloaded!")

    def create_task(self, t):
        self.tasks.append(t)

    def big_message(self, msg: str):
        self.msg = msg
        self.msg_time = 1

    def update(self, dt: float):
        f, hit = self.editor.hitcast()
        cur_color = self.editor.map.get_color(hit.x, hit.y, hit.z)

        # Draw tool information
        p = self.font.draw(f"Cursor: {hit}\n", vec2(30, 10), vec3(1, 0, 0), vec2(1, 1), Align.TOP_LEFT)
        p = self.font.draw(f"Color@Cursor: {cur_color}\n", p, vec3(cur_color) / 255.0)
        p = self.font.draw(f"Tool: {self.tool.name}\n", p, vec3(1, 0, 0))
        p = self.font.draw(f"Tool Mode: {self.mode.value}\n", p, vec3(1, 0, 0))
        p = self.font.draw(f"Tool Color: {self.color}\n", p, vec3(self.color) / 255.0)

        self.tool.update(dt)

        self.msg_time = max(0, self.msg_time - dt)

        if self.msg_time > 0:
            self.font.draw(f"Selected Mode: {self.msg}", vec2(960, 617), vec3(1, 0, 0), vec2(1.25), Align.BOTTOM_CENTER)

        # for i in range(len(self.tasks) - 1, -1, -1):
        #     try:
        #         next_call = next(self.tasks[i]) or 0  # TODO: next_call delay
        #     except StopIteration:
        #         del self.tasks[i]

    def run_loop(self):
        # self.loop.stop()
        # self.loop.run_forever()
        pass

    def draw(self):
        self.tool.draw()

    def on_key(self, scancode: Key, modifiers: Keymod, pressed: bool):
        self.tool.on_key(scancode, modifiers, pressed)
        print(scancode, modifiers, pressed)

        if not pressed: return

        if scancode == Key.Q:
            self.color = random_color()
        elif scancode == Key.E:
            self.mode = next(self.mode_cycle)
            self.big_message(self.mode.value)
        elif scancode == Key._1:
            self.tool = tools.BlockLineTool(self)
        elif scancode == Key._2:
            self.tool = tools.BlockBoxTool(self)
        elif scancode == Key._3:
            self.tool = tools.FaceTool(self)
        elif scancode == Key.F1:
            self.editor.save_to("vxl/editor.vxl")

    def on_mouse_button(self, button: Button, pressed: bool):
        self.tool.on_mouse_button(button, pressed)
        print(button, pressed)

    def on_mouse_scroll(self, vertical: int, horizontal: int):
        self.tool.on_mouse_scroll(vertical, horizontal)

    # def on_text_typing(self, text: str):
    #     pass

    def on_text_finished(self, text: str, cancelled: bool):
        if cancelled: return

        self.command_manager.on_text_input(text)
        # variables = globals().copy()
        # variables.update(locals())
        #
        # # scope = {"self": self, "text": text}
        # exec(text, variables)


class BaseCommands:
    def __init__(self, parent: 'EditorScript'):
        self.parent = parent

    @commands.command(name='fog')
    def fog_color(self, r: int, g: int, b: int):
        self.parent.editor.uniforms.fog_color = glm.vec3(r, g, b) / 255

    @commands.overload('fog')
    def fog_enable(self, enabled: bool):
        unis = self.parent.editor.uniforms
        if enabled:
            unis.fog_start = 64
            unis.fog_end = 128
        else:
            unis.fog_start = 512
            unis.fog_end = 1024

    @commands.command()
    def color(self, r: int, g: int, b: int):
        self.parent.color = u8vec3(r, g, b)

    @commands.command()
    def map(self, file: str):
        if not file.endswith(".vxl"): file += ".vxl"
        with open(f"vxl/{file}", 'rb') as f:
            self.parent.editor.map.read(f.read())

    @commands.command()
    def model(self, file: str):
        self.parent.tool = tools.ModelTool(self.parent, file)
