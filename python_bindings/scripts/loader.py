import importlib
# import logging
# import os
# import sys
from typing import Dict
from collections import OrderedDict

# import pybind11_stubgen

import ace
from ace.editor import MapEditor


class Script:
    def __init__(self, editor: MapEditor):
        self.editor: MapEditor = editor

    def deinit(self):
        pass


class ScriptLoader:
    HOOKS = ("update", "draw", "on_key", "on_mouse_button", "on_mouse_motion",
             "on_mouse_scroll", "on_text_typing", "on_text_finished")

    def __init__(self, editor: MapEditor):
        self.editor: MapEditor = editor
        self.scripts: Dict[str, Script] = OrderedDict()

        self.updates = []
        self.draws = []
        self.on_keys = []
        self.on_mouse_buttons = []
        self.on_mouse_motions = []
        self.on_mouse_scrolls = []
        self.on_text_typings = []
        self.on_text_finisheds = []

    def load_scripts(self, reload=False):
        self.unload_scripts()

        for script_name in ["editor"]:  # TODO
            module = importlib.import_module(script_name)
            if reload:
                importlib.reload(module)
            script = module.init(self.editor)
            if not isinstance(script, Script):
                raise TypeError(f"Script {script_name} did not return a Script instance!")

            self.load_functions(script)

            self.scripts[script_name] = script

    def load_functions(self, script: Script):
        for func_name in self.HOOKS:
            func = getattr(script, func_name, None)
            if func:
                getattr(self, func_name + "s").append(func)


    def unload_functions(self):
        for func_name in self.HOOKS:
            getattr(self, func_name + "s").clear()

    def unload_scripts(self):
        self.unload_functions()
        for script in reversed(self.scripts.values()):
            script.deinit()
        self.scripts.clear()

    def update(self, *args, **kwargs):
        for f in self.updates: f(*args, **kwargs)

    def draw(self, *args, **kwargs):
        for f in self.draws: f(*args, **kwargs)

    def on_key(self, *args, **kwargs):
        for f in self.on_keys: f(*args, **kwargs)

    def on_mouse_button(self, button: int, pressed: bool):
        # I could fix this in C++, but SDL doesn't actually expose an
        # SDL_Button enum, I just made one in-line the SDL binding code.
        # I'd rather not expose that publicly to use it.
        for f in self.on_mouse_buttons: f(ace.Button(button), pressed)

    def on_mouse_motion(self, *args, **kwargs):
        for f in self.on_mouse_motions: f(*args, **kwargs)

    def on_mouse_scroll(self, *args, **kwargs):
        for f in self.on_mouse_scrolls: f(*args, **kwargs)

    def on_text_typing(self, *args, **kwargs):
        for f in self.on_text_typings: f(*args, **kwargs)

    def on_text_finished(self, *args, **kwargs):
        for f in self.on_text_finisheds: f(*args, **kwargs)

#
# def main():
#     stderr_handler = logging.StreamHandler(sys.stderr)
#     handlers = [stderr_handler]
#
#     logging.basicConfig(
#         level=logging.DEBUG,
#         format='[%(asctime)s] {%(filename)s:%(lineno)d} %(levelname)s - %(message)s',
#         handlers=handlers
#     )
#
#     output_path = "stubs3"
#
#     if not os.path.exists(output_path):
#         os.mkdir(output_path)
#
#     with pybind11_stubgen.DirectoryWalkerGuard(output_path):
#         for _module_name in ["ace"]:
#             _module = pybind11_stubgen.ModuleStubsGenerator(_module_name)
#             _module.parse()
#             _module.stub_suffix = "-stubs"
#             _module.write_setup_py = False
#             pybind11_stubgen.recursive_mkdir_walker(_module_name.split(".")[:-1], lambda: _module.write())
#
# main()
