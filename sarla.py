from termcolor import colored 
import click
from sarla.server.common import settings
import sarla.server.server as server
import sarla.client.client as client


@click.command()
@click.argument("status")
def command(status):
    if status == "client":
        client.run()

    elif status == "server":
        settings.init_globals()
        server.run()

    else:
        click.echo(colored("Error: Argument unrecognized", "red"))


if __name__ == "__main__":
    command()
