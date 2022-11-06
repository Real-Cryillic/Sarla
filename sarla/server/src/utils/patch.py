from sarla.server.src.utils import settings


def convert_command_to_patch(command):
    settings.command_patch_list = {
        "whoami": 0xAA,
        "pwd": 0xAB,
        "hostname": 0xAC,
        "username": 0xAD,
        "exit": 0xAE
    }
    try:
        command_patch = settings.command_patch_list[command]
    except:
        print("Error occurred trying to patch command value")
    return command_patch
