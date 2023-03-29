from prompt_toolkit import PromptSession, prompt
from prompt_toolkit.styles import Style
from prompt_toolkit.completion import NestedCompleter
from prompt_toolkit.auto_suggest import AutoSuggestFromHistory
import requests
import json
from rich.console import Console
from rich.theme import Theme
from rich.padding import Padding
from rich.table import Table

from sarla.src.client.common import ascii

session = PromptSession()

dracula = Theme(
    {
        "success": "#50fa7b",
        "error": "#ff5555",
        "warning": "#ffb86c",
        "default": "#6272a4",
    }
)
console = Console(theme=dracula)

help_menu = {
    "help": "Print help menu",
    "listen": "Start HTTP listener",
    "quit": "Quit the current session",
    "clear": "Clear current console window",
    "select": "Set a global variable",  # Drop down of available agents
    "agents": "Print a table of current beacons",
    "tasks": "Print a table of currently queued tasks",  # Qeue
    "history": "Print the history for the current session",
}

message = [
    ("class:server", " Sarla / ")
]

style = Style.from_dict(
    {
        # User input (default text)
        "": "#f8f8f2",
        # Prompt
        "host": "#50fa7b",
        "port": "#8be9fd",
        "authflag": "ansimagenta",
        "default": "#f8f8f2",
    }
)

message_completer = NestedCompleter.from_nested_dict(
    {
        "help": None,
        "listen": None,
        "quit": None,
        "clear": None,
        "select": None,  # Drop down of available agents
        "jobs": None,
        "tasks": None,
        "history": None,
    }
)

def help():
    """
    Print a help menu using the rich library table
    Menu entries are defined from help_menu glopbal
    """
    # Initialize rich table
    table = Table(expand=True)

    # Add table headers
    table.add_column("Command", style="#f8f8f2")
    table.add_column("Description")

    # Add help menu entries as rows
    for key in help_menu:
        table.add_row(key, help_menu[key])

    # Add padding to table
    output = Padding(table, (0, 1, 0, 1), style="default")
    console.print(output)

def run():
    print("\033c")  # Clear the current terminal
    ascii.print_ascii(console)

    while True:
        input = session.prompt(
            message,
            style=style,
            completer=message_completer,
            auto_suggest=AutoSuggestFromHistory(),
        )

        try:
            if input == "":
                continue

            input = input.split()
            command = input[0].lower()

            if command == "help":
                help()

            elif command == "agents":
                url = "http://127.0.0.1:5000/api/client/agents"
                response = requests.get(url)
                response_json = json.dumps(response.json(), indent=4)

                console.print(response_json)

        except IndexError:
            output = Padding("[error]Error:[/error] could not process input", (1, 2), style="default")
            console.print(output)