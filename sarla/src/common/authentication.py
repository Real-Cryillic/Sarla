import random
import string

global keyword
keyword = "schistosomiasis"

def generate_id():
    id = (random.SystemRandom().choice(string.ascii_uppercase + string.digits) for _ in range(8))
    id = 'A-' + ''.join(id)
    return id

def generate_key(keyword):
    key = str(abs(hash(keyword)))
    return key