from sarla.server.src.utils import settings
import requests


def update_pwnboard(address_tuple):
    ADDRESS_INDEX = 0
    PORT_INDEX = 1
    address = address_tuple[0]
    port = address_tuple[1]

    application_name = 'Sarla'
    data = {'ip': address, 'application': application_name}
    try:
        req = requests.post(settings.pwnboard, json=data, timeout=3)
        print("Successfully updated pwnboard with address", address)
        return True
    except Exception as E:
        print(E)
        return False
