AGENT_KEY = "boondoggle"


def generate_key():
    key = hash(AGENT_KEY)
    return key


def auth(data_dict):
    key = data_dict["key"]
    if key == AGENT_KEY:
        beacon_key = abs(generate_key())
        data_dict["key_hash"] = beacon_key
    # else:
    # response

    return beacon_key
