from base64 import b64decode
from rich.console import Console
from rich.theme import Theme
from rich.padding import Padding
from sarla.server.src.register import register

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

def process_agent(data, dictionary):
    unformatted_data = str(b64decode(data))
    unformatted_data = unformatted_data[2:len(unformatted_data)]
    unformatted_data = unformatted_data[:-1]
    seperated_data = unformatted_data.split(":")

    beacon_type = seperated_data[0]
    data = seperated_data[1]

    if beacon_type == 'register':
        key = register(data, dictionary)
        return key

    elif beacon_type == 'beacon':
        return "beacon"
    elif beacon_type == 'output':
        return "output"
    else:
        output = Padding("Error: unknown beacon type attempted to connect", (0, 2), style="error")
        console.print(output)
