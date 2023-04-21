import random
import string

global keyword
keyword = "schistosomias"


def generate_id():
    id = (
        random.SystemRandom().choice(string.ascii_uppercase + string.digits)
        for _ in range(8)
    )
    id = "A-" + "".join(id)
    return id


def generate_key(keyword):
    hash = 0
    for char in keyword:
        hash += ( hash * 281 ^ ord(char) * 997) & 0xFFFFFFFF
    return hash
