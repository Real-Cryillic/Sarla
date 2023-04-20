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

from sarla.src.client.common import ascii, tables

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
    "back": "Remove current agent",
}

style = Style.from_dict(
    {
        # User input (default text)
        "": "#f8f8f2",
        # Prompt
        "host": "#50fa7b",
        "port": "#8be9fd",
        "authflag": "ansimagenta",
        "default": "#f8f8f2",
        "agent": "#8be9fd",
        "server": "#44475a",
    }
)

global active_agent
active_agent = ""


def send_get(url):
    response = requests.get(url)

    response_json = json.dumps(response.json(), indent=4)
    data = json.loads(response_json)

    return data


def send_post(url, data):
    return requests.post(url, json=data)


message_completer = NestedCompleter.from_nested_dict(
    {
        "help": None,
        "listen": None,
        "quit": None,
        "clear": None,
        # "select": get_agent_map(),  # Drop down of available agents
        "jobs": None,
        "tasks": None,
        "history": None,
        "back": None,
    }
)

command_list = ["whoami", "hostname"]


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


def set_message_prompt():
    if active_agent != "":
        return [("class:agent", " " + active_agent), ("class:default", " / ")]
    else:
        return [("class:server", " Sarla"), ("class:default", " / ")]


def set_message_completer():
    data = send_get("http://127.0.0.1:5000/api/client/agents")

    list = []
    map = {}

    for key in data:
        for subkey, value in key.items():
            if subkey == "id":
                list.insert(value)
            else:
                pass

    for id in list:
        map.update({id: None})

    message_completer = NestedCompleter.from_nested_dict(
        {
            "help": None,
            "listen": None,
            "quit": None,
            "clear": None,
            "jobs": None,
            "tasks": None,
            "history": None,
            "back": None,
            "agents": None,
            "select": map,
        }
    )

    return message_completer


def run():
    print("\033c")  # Clear the current terminal
    ascii.print_ascii(console)

    while True:
        input = session.prompt(
            set_message_prompt,
            style=style,
            completer=message_completer,
            auto_suggest=AutoSuggestFromHistory(),
        )

        try:
            if input == "":
                continue

            input = input.split(" ", 1)
            command = input[0].lower()

            if command == "help":
                help()

            elif command == "agents":
                data = send_get("http://127.0.0.1:5000/api/client/agents")

                print(tables.create_table(data).table)

            elif command == "select":
                if len(input) > 1:
                    data = send_get("http://127.0.0.1:5000/api/client/agents")

                    for key in data:
                        for subkey, value in key.items():
                            if subkey == "id":
                                if value == input[1]:
                                    global active_agent
                                    active_agent = value
                            else:
                                pass
                else:
                    output = Padding(
                        "[error]Error:[/error] please provide a parameter",
                        (1, 2),
                        style="default",
                    )
                    console.print(output)

            elif command == "back":
                active_agent = ""

            elif active_agent != "":
                if command in command_list:
                    """
                    Handle queue errors here
                    """
                    if len(input) > 1:
                        json = {"id": active_agent, "command": command, "param": input[1]}
                    else:
                        json = {"id": active_agent, "command": command}

                    send_post("http://127.0.0.1:5000/api/client/queue", json)

                if command == "command":
                    if len(input) > 1:
                        json = {"id": active_agent, "command": input[1]}

                    send_post("http://127.0.0.1:5000/api/client/queue", json)

                else:
                    output = Padding(
                        "[error]Error:[/error] unknown command", (1, 2), style="default"
                    )
                    console.print(output)

            else:
                output = Padding(
                    "[error]Error:[/error] unknown command", (1, 2), style="default"
                )
                console.print(output)

        except IndexError:
            output = Padding(
                "[error]Error:[/error] could not process input", (1, 2), style="default"
            )
            console.print(output)
