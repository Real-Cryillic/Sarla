import requests


def update_pwnboard(data):
    address = data[0]
    port = data[1]

    application_name = "Sarla"
    data = {"ip": address, "application": application_name}

    try:
        req = requests.post("http://pwnboard.win/pwn/boxaccess", json=data, timeout=3)
    except Exception as E:
        print(E)
