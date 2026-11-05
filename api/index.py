
import os
import sys
from urllib.parse import urlparse, parse_qs

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

if ROOT not in sys.path:
    sys.path.insert(0, ROOT)

from app import CompressionHandler


class handler(CompressionHandler):

    def restore_route(self):
        parsed = urlparse(self.path)
        route = parse_qs(parsed.query).get("route")

        if route:
            self.path = route[0]

    def do_GET(self):
        self.restore_route()
        super().do_GET()

    def do_POST(self):
        self.restore_route()
        super().do_POST()
