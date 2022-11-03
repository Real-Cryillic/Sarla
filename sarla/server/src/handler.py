from base64 import b64decode
from sarla.server.src.registration import register


def process_agent(data):
    agent_dict = dict()
    unformatted_data = str(b64decode(data))
    unformatted_data = unformatted_data[2:len(unformatted_data)]
    unformatted_data = unformatted_data[:-1]
    seperated_data = unformatted_data.split(':')

    identifier = seperated_data[0]
    data = seperated_data[1]

    if identifier == 'register':
        key = register(data, agent_dict)
        print(agent_dict)
        return key
    else:
        for key in agent_dict:
            if identifier == key:
                pass