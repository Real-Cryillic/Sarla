import random
import string

def generate_key(keyword):
    key = abs(hash(keyword))
    return key


def generate_id():
    return 'A-' + ''.join(
        random.SystemRandom().choice(string.ascii_uppercase + string.digits)
        for _ in range(8))