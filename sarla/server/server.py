from sarla.server.common import settings

def run():
    print("server")
    settings.host = "0.0.0.0"
    settings.port = 1337

    print(settings.host)
    print(settings.port)
    print("hello")

    input("Press enter to quit... ")