from base64 import b64decode
from rich.console import Console
from rich.theme import Theme
from rich.padding import Padding
from sarla.server.src.negotiate import negotiate
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

    print(beacon_type)
    print(data)

    if beacon_type == "0":
       key = negotiate(data, dictionary)
       return key 

    elif beacon_type == "1":
        key = register(data, dictionary)
        return key

    elif beacon_type == "2":
        beacon_origin = ""
        for key in dictionary:
            if str(data) == str(dictionary[key]['key']):
                beacon_origin = key
        if beacon_origin != "":
            output = Padding("[success]" + beacon_origin + "[success] checked in[/success] ", (1, 2), style="#f8f8f2")
            console.print(output)
        else: 
            output = Padding("Error: unknown beacon type attempted to connect", (0, 2), style="error")
            console.print(output)

    elif beacon_type == "3":
        return "output"
    else:
        output = Padding("Error: unknown beacon type attempted to connect", (0, 2), style="error")
        console.print(output)
