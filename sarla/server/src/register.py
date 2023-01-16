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
keyword = "boondoggle" # This should be updated as a global

dictionary_format = {
    'user': '',
    'host': '',
    'pid': '',
    'build': '',
    'name': '',
    'domain': '',
    'arch': '',
    'product': '',
    'keyword': ''
}

def register(data, dictionary):
    job_dictionary = dictionary_format
    agent_information = data.split(',')

    agent_id = auth.generate_id()
    output = Padding("[success]New agent checked in:[/success] " + agent_id, (1, 2), style="#f8f8f2")
    console.print(output)

    i = 0
    for key in job_dictionary:
        job_dictionary[key] = agent_information[i]
        i += 1

    if job_dictionary['keyword'] == keyword:
        key = auth.generate_key(job_dictionary['keyword'])
        
        job_dictionary['id'] = agent_id
        job_dictionary['key'] = key

        dictionary[job_dictionary['id']] = job_dictionary

        return key

    else:
        return False