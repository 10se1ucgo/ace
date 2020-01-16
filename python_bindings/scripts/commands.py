import sys
import shlex
import inspect
import traceback
from typing import Dict, Callable


def command(name=None, aliases=None):
    def decorator(func):
        return Command(name or func.__name__, func, aliases)
    return decorator


def overload(name=None):
    def decorator(func):
        return Overload(name or func.__name__, func)
    return decorator


class NotEnoughArguments(TypeError):
    pass


class IncorrectParameters(TypeError):
    pass


class Overload:
    def __init__(self, name: str, func: callable):
        self.name = name
        self.func = func

class Command:
    def __init__(self, name: str, func: callable, aliases=None):
        self.name = name
        self.aliases = aliases or []
        self.instance = None
        self.overloads = [func]

    def __call__(self, msg: list):
        for func in self.overloads:
            try:
                if self.instance is not None:
                    return func(self.instance, *self.parse_command(msg, inspect.signature(func)))
                return func(*self.parse_command(msg, inspect.signature(func)))
            except (IncorrectParameters, NotEnoughArguments) as e:
                pass

    def parse_command(self, msg: list, signature: inspect.Signature) -> list:
        args = []

        print(signature.parameters)

        parameters = iter(signature.parameters)
        if self.instance is not None:
            next(parameters)  # skip the self argument (the script that owns the command)

        for x, param_name in enumerate(parameters):
            param: inspect.Parameter = signature.parameters[param_name]
            if param.annotation is param.empty:
                param_type = type(param.default) if param.default is not param.empty else str
            else:
                param_type = param.annotation

            converter = _converters.get(param_type, param_type)
            try:
                arg = msg[x]
            except IndexError:
                if param.default is param.empty:
                    raise NotEnoughArguments(f"Not enough commands supplied to {self.name}") from None
                arg = param.default
            else:
                try:
                    arg = converter(arg)
                except (TypeError, ValueError):
                    raise IncorrectParameters(
                        f'Unable to convert argument `{param_name}` "{msg[x]}" to "{param_type}" in command {self.name}'
                    )
            args.append(arg)

        print(args)

        return args


class CommandManager:
    COMMAND_PREFIX = "/"

    def __init__(self):
        self.commands: Dict[str, Command] = {}
        self.aliases: Dict[str, str] = {}

    def add_commands(self, klass):
        for name, command in inspect.getmembers(klass, lambda member: isinstance(member, Command)):
            command.instance = klass
            self.commands[command.name] = command
            for alias in command.aliases:
                self.aliases[alias] = command.name

        for name, overload in inspect.getmembers(klass, lambda member: isinstance(member, Overload)):
            self.commands[self.aliases.get(overload.name, overload.name)].overloads.append(overload.func)

    def remove_commands(self, klass):
        for command in list(self.commands.values()):
            if command.instance is klass:
                command.instance = None
                for alias in command.aliases:
                    self.aliases.pop(alias)
                self.commands.pop(command.name)

    def on_text_input(self, message: str):
        if message.startswith(self.COMMAND_PREFIX):
            message = message[len(self.COMMAND_PREFIX):]

        fragments = shlex.split(message)
        if not fragments:
            return

        command_name = fragments[0]
        command = self.commands.get(self.aliases.get(command_name, command_name))
        if not command:
            print("That command doesn't exist.")

        try:
            command(fragments[1:])
        except Exception:
            print(f"Error executing command {command.name}, not enough or malformed arguments")
            print(f"Ignoring error in command {command.name}", file=sys.stderr)
            traceback.print_exc()


_converters: Dict[str, Callable] = {}
def register_converter(param_type):
    def decorator(func):
        _converters[param_type] = func
        return func
    return decorator


@register_converter(bool)
def to_bool(parameter: str):
    parameter = parameter.lower()
    if parameter in ("yes", "on", "true", "1"):
        return True
    elif parameter in ("no", "off", "false", "0"):
        return False
    raise ValueError


@register_converter(int)
def to_int(parameter: str):
    # Wrapper to allow parsing of hex, octal, and binary inputs.
    return int(parameter, 0)

