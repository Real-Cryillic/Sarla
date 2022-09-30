import threading
from http.server import HTTPServer, BaseHTTPRequestHandler

host = "0.0.0.0"
port = 8080


class requests(BaseHTTPRequestHandler):
    def _set_headers(self):
        self.send_response(200)
        self.send_header("Conent-type", "text/html")
        self.end_headers()

    def _html(self, message):
        content = f"{message}"
        return content.encode("utf8")

    def do_GET(self):
        self._set_headers()
        self.wfile.write(self._html("404 Not Found"))


def server():
    server = HTTPServer((host, port), requests)
    thread = threading.Thread(None, server.serve_forever)
    thread.daemon = True
    thread.start()
