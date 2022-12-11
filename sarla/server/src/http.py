import threading
from http.server import HTTPServer, BaseHTTPRequestHandler
from rich.console import Console
from rich.theme import Theme
from rich.padding import Padding
from sarla.server.src.handler import process_agent

# rich globals
dracula = Theme(
    {
        "success": "#50fa7b",
        "error": "#ff5555",
        "warning": "#ffb86c",
        "default": "#6272a4",
    }
)
console = Console(theme=dracula)

# other globals
host = "0.0.0.0"
port = 1337
agent_dictionary = dict()

class requests(BaseHTTPRequestHandler):

    def _set_headers(self):
        self.send_response(200)
        self.send_header("Content-type", "text/html")
        self.end_headers()

    def _html(self, message):
        content = f"{message}"
        return content.encode("utf8")

    def do_GET(self):
        self._set_headers()
        self.wfile.write(self._html("404 Not Found"))

    def do_POST(self):
        post_data = ((self.rfile.read(int(self.headers["content-length"]))
                     ).decode("utf-8")).rstrip("\r\n\r\n\0")
        cookie = process_agent(post_data, agent_dictionary)
        output = Padding("Response sent to " + self.client_address[0] + ": " + str(cookie), (1, 2), style="#f8f8f2")
        console.print(output)
        self._set_headers()
        self.wfile.write(self._html(cookie))

    def log_message(self, format, *args):
        return


def start_server(console):
    server = HTTPServer((host, port), requests)
    thread = threading.Thread(None, server.serve_forever)
    thread.daemon = True
    thread.start()