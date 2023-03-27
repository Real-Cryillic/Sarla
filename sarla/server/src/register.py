from rich.console import Console
from rich.theme import Theme
from rich.padding import Padding
from sarla.server.common import authentication as auth

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

def register(data, dictionary):
    agent_information = data.split(',')

    for id in dictionary:
        if (dictionary[id]['key'] == agent_information[0]):
            output = Padding("[success]Agent registered:[/success] " + str(dictionary[id]['id']), (1, 2), style="#f8f8f2")
            console.print(output)

            i = 1
            for field in dictionary[id]:
                if dictionary[id][field] == "":
                    dictionary[id][field] = agent_information[i]
                    i += 1
            return True
        else:
            return False