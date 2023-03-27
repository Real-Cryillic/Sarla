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

# other globals
keyword = "schistosomiasis" 

dictionary_format = {
    'host': '',
    'user': '',
    'pid': '',
    'name': '',
    'arch': '',
    'key': '',
    'id': ''
}

def negotiate(data, dictionary):
    if data == keyword:
        job_dictionary = dictionary_format
        agent_id = auth.generate_id()

        output = Padding("[success]New agent attempted to negotiate key:[/success] " + agent_id, (1, 2), style="#f8f8f2")
        console.print(output)

        key = auth.generate_key(data)

        job_dictionary['id'] = agent_id
        job_dictionary['key'] = key

        dictionary[job_dictionary['id']] = job_dictionary

        return key
    else:   
        return False