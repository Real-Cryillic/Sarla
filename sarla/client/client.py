from sarla.client.utils import ascii
from prompt_toolkit import PromptSession, prompt
from prompt_toolkit.styles import Style
from prompt_toolkit.completion import NestedCompleter
from prompt_toolkit.auto_suggest import AutoSuggestFromHistory
from rich.console import Console
from rich.theme import Theme
from rich.padding import Padding
from rich.table import Table

# prompt_toolkit globals
session = PromptSession()

# rich globals
dracula = Theme(
    {
        "success": "#50fa7b",
        "error": "#ff5555",
        "warning": "#ffb86c",
        "default": "#6272a4",
    }
)
console = Console(theme=dracula)

# other globals
host = "0.0.0.0"
port = 1337
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

"""
Color theme:
Background	    #282a36	
Current Line	#44475a	
Foreground	    #f8f8f2	
Comment	        #6272a4	
Cyan	        #8be9fd	
Green	        #50fa7b	
Orange	        #ffb86c	
Pink	        #ff79c6	
Purple	        #bd93f9	
Red	            #ff5555	
Yellow	        #f1fa8c
"""

# prompt message [Sarla / ( 0.0.0.0 : 1337 )  >]
message = [ 
    ("class:server", " Sarla / "),
    ("class:default", "( "),
    ("class:host", host),
    ("class:default", " : "),
    ("class:port", str(port)),
    ("class:default", " ) "),
]

# prompt color/style
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

# define tab completer commands
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


def quit():
    """
    Confirm user wants to exit the prompt session and quit the terminal if confirmed
    """
    input = session.prompt("Are you sure you want to quit? (y/n) Default (y): ")
    if input.lower() == "n":
        return False
    else:
        return True


def run():
    """
    Main function for prompt session and command parsing
    """
    print("\033c")  # Clear the current terminal
    ascii.print_ascii(console) # Print ascii logo

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
            COMMAND = input[0].lower()

            if COMMAND == "help":
                help()

            elif COMMAND == "quit":
                if quit() == True:
                    return 0

            elif COMMAND == "exit":
                if quit() == True:
                    return 0

            elif COMMAND == "clear":
                print("\033c")

            elif COMMAND == "listen":
                output = Padding("Warning: command in development", (1, 2), style="warning")
                console.print(output)

            elif COMMAND == "select":
                output = Padding("Warning: command in development", (1, 2), style="warning")
                console.print(output)

            elif COMMAND == "agents":
                output = Padding("Warning: command in development", (1, 2), style="warning")
                console.print(output)

            elif COMMAND == "tasks":
                output = Padding("Warning: command in development", (1, 2), style="warning")
                console.print(output)

            elif COMMAND == "history":
                output = Padding("Warning: command in development", (1, 2), style="warning")
                console.print(output)

            else:
                output = Padding("Error: unknown command", (1, 2), style="error")
                console.print(output)
        except IndexError:
            output = Padding("Error: could not process input", (1, 2), style="error")
            console.print(output)