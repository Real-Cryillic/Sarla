from termcolor import colored 
import click
import sarla.server.server as server
import sarla.client.client as client


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
