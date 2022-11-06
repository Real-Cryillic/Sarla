def init():
    global agent_table
    global agent_active
    global agent_dict
    global command_queue_table
    command_queue_table = dict()
    global command_patch_list
    command_patch_list = {
        "whoami": 0xAA,
        "pwd": 0xAB,
        "hostname": 0xAC,
        "username": 0xAD,
        "exit": 0xAE
    }