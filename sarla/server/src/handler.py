from base64 import b64decode
from sarla.server.src.registration import register
from sarla.server.src.utils import settings
from sarla.server.src.utils import patch


def process_agent(data, dictionary):
    unformatted_data = str(b64decode(data))
    unformatted_data = unformatted_data[2:len(unformatted_data)]
    unformatted_data = unformatted_data[:-1]
    seperated_data = unformatted_data.split(':')

    identifier = seperated_data[0]
    data = seperated_data[1]

    if identifier == 'register':
        key = register(data, dictionary)
        print(dictionary)
        return key
    else:
        for key in dictionary:
            print(identifier)
            print(dictionary[key]['key'])
            if str(identifier) == str(dictionary[key]['key']):
                if str(data) == "beacon":
                    for command_key, command_value in settings.command_queue_table:
                        if len(command_value) > 0:
                            command_patch = patch.convert_command_to_patch(command_value)
                            del settings.command_queue_table[command_key]
                            return command_patch
                    pass
                else:
                    pass
                return "This is a command"