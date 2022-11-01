from sarla.server.src.registration import register

def process_agent(data):
    register_dict = register(data)
    return register_dict