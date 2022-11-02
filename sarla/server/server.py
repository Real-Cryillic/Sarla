from sarla.server.src.http_server import server

from prompt_toolkit import prompt, Application, PromptSession
from prompt_toolkit.styles import Style
from prompt_toolkit.completion import NestedCompleter
from prompt_toolkit.auto_suggest import AutoSuggestFromHistory

# global variables
session = PromptSession()
host = "0.0.0.0"
port = "443"
auth_flag = "random"

# prompt color/style
style = Style.from_dict({
    # User input (default text)
    "": "#884444",
    # Prompt
    "server": "#884444",
    "pipe": "ansicyan",
    "host": "ansired",
    "port": "ansigreen",
    "authflag": "ansimagenta",
    "default": "#884444",
})

# prompt message
message = [
    ("class:server", "( Sarla )"),
    ("class:default", "( "),
    ("class:host", host),
    ("class:default", " : "),
    ("class:port", port),
    ("class:default", " : "),
    ("class:authflag", auth_flag),
    ("class:default", " )"),
    ("class:pipe", " > "),
]

message_completer = NestedCompleter.from_nested_dict({
    "help": None,
    "set": {
        "version": None,
        "clock": None,
        "ip": {
            "interface": {"brief"}
        }
    },
    "listen": None,
    "quit": None,
    "clear": None,
})


def quit():
    input = session.prompt(
        "Are you sure you want to quit? (y/n) Default (y): ")
    if input.lower() == "n":
        return False
    else:
        return True


def start():
    print("\033c")  # Clear current terminal

    # CLI prompt handler
    while True:
        input = session.prompt(
            message,
            style=style,
            completer=message_completer,
            auto_suggest=AutoSuggestFromHistory(),
        ).split()
        try:
            if input[0] == "help":
                print("help")
            elif input[0] == "quit":
                if quit() == True:
                    return 0
            elif input[0] == "clear":
                print("\033c")
            elif input[0] == "set":
                print("set")
            elif input[0] == "listen":
                server()
            else:
                print("Invalid command")
        except IndexError:
            print("Enter a command or 'quit' to exit the program.")
