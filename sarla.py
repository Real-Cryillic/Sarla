import click
from termcolor import colored

from sarla.src.server import server
from sarla.src.client import client

@click.command()
@click.argument("status")
def command(status):
    if status == "client":
        client.run()

    elif status == "server":
        server.run()

    else:
        click.echo(colored("Error: Argument unrecognized", "red"))

if __name__ == "__main__":
    command()