import random
import string

AGENT_KEY = "boondoggle"


def generate_key():
    key = hash(AGENT_KEY)
    return key


def generate_id():
    return 'A-' + ''.join(
        random.SystemRandom().choice(string.ascii_uppercase + string.digits)
        for _ in range(4))


def auth(data_dict):
    beacon_key = abs(generate_key())
    agent_id = generate_id()
    data_dict["agent_id"] = agent_id
    data_dict["key_hash"] = beacon_key

    return beacon_key, agent_id
