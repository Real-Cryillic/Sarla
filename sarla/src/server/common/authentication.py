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
    hash = (
        random.SystemRandom().choice(string.ascii_uppercase + string.ascii_lowercase + string.digits)
        for _ in range(16)
    )
    return str(hash)
