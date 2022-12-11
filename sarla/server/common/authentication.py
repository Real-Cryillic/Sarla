import random
import string

def generate_id():
    id = (random.SystemRandom().choice(string.ascii_uppercase + string.digits) for _ in range(8))
    id = 'A-' + ''.join(id)
    return id

def generate_key(keyword):
    key = abs(hash(keyword))
    return key