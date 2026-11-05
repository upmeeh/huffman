"""
Huffman Text Compression — standard-library-only web application.

Run:
    python app.py
Then open http://localhost:8000 in a browser.

Sample input:
    BANANA BANDANA

Sample output (the exact bit string depends on deterministic tie-breaking):
    frequencies: {"A": 6, "N": 4, "B": 2, " ": 1, "D": 1}
    codes:       {"A": "0", "N": "10", " ": "1100", "D": "1101", "B": "111"}
    decoded:     "BANANA BANDANA"

Complexity analysis:
    Let n be the number of characters in the input and k the number of
    distinct characters. Counting frequencies takes O(n) time. Building the
    Huffman tree performs k - 1 insert/remove-min operations; each custom
    min-heap operation costs O(log k), so tree construction is O(k log k).
    A tree walk visits O(k) nodes, making code generation O(k). Encoding and
    decoding are O(n) and O(number of encoded bits), respectively. Overall,
    the algorithm is O(n + k log k + number of encoded bits) time.
    The frequency map, heap, tree, code map, and encoded output use
    O(n + k) space in the usual text representation.

The MinHeap below is intentionally implemented from scratch rather than
using Python's heapq module, so its data-structure operations are visible.
"""

from __future__ import annotations

import json
import os
from collections import Counter
from http import HTTPStatus
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from typing import Any, Callable, Generic, TypeVar
from urllib.parse import urlparse


T = TypeVar("T")


class MinHeap(Generic[T]):
    """A binary min-heap with an explicit comparator."""

    def __init__(self, key: Callable[[T], Any] | None = None) -> None:
        self._items: list[T] = []
        self._key = key or (lambda item: item)

    def __len__(self) -> int:
        return len(self._items)

    def push(self, item: T) -> None:
        self._items.append(item)
        self._sift_up(len(self._items) - 1)

    def pop_min(self) -> T:
        if not self._items:
            raise IndexError("cannot pop from an empty heap")
        minimum = self._items[0]
        last = self._items.pop()
        if self._items:
            self._items[0] = last
            self._sift_down(0)
        return minimum

    def _sift_up(self, index: int) -> None:
        while index > 0:
            parent = (index - 1) // 2
            if self._key(self._items[parent]) <= self._key(self._items[index]):
                break
            self._items[parent], self._items[index] = (
                self._items[index],
                self._items[parent],
            )
            index = parent

    def _sift_down(self, index: int) -> None:
        size = len(self._items)
        while True:
            left = index * 2 + 1
            right = left + 1
            smallest = index

            if left < size and self._key(self._items[left]) < self._key(
                self._items[smallest]
            ):
                smallest = left
            if right < size and self._key(self._items[right]) < self._key(
                self._items[smallest]
            ):
                smallest = right
            if smallest == index:
                return
            self._items[index], self._items[smallest] = (
                self._items[smallest],
                self._items[index],
            )
            index = smallest


class BinaryTree:
    """A Huffman node; leaves contain a character and internal nodes do not."""

    def __init__(
        self,
        frequency: int,
        character: str | None = None,
        left: BinaryTree | None = None,
        right: BinaryTree | None = None,
    ) -> None:
        self.frequency = frequency
        self.character = character
        self.left = left
        self.right = right

    @property
    def is_leaf(self) -> bool:
        return self.character is not None


def build_huffman_tree(frequencies: dict[str, int]) -> BinaryTree | None:
    """Build a deterministic Huffman tree using the greedy strategy."""
    heap: MinHeap[tuple[int, int, BinaryTree]] = MinHeap(
        key=lambda item: (item[0], item[1])
    )
    sequence = 0
    for character, frequency in sorted(frequencies.items()):
        heap.push((frequency, sequence, BinaryTree(frequency, character)))
        sequence += 1

    if not heap:
        return None
    while len(heap) > 1:
        left_frequency, _, left = heap.pop_min()
        right_frequency, _, right = heap.pop_min()
        parent = BinaryTree(left_frequency + right_frequency, left=left, right=right)
        heap.push((parent.frequency, sequence, parent))
        sequence += 1
    return heap.pop_min()[2]


def build_huffman_steps(
    frequencies: dict[str, int],
) -> list[dict[str, Any]]:
    """Return visualizable snapshots of the greedy heap-building process."""
    heap: MinHeap[tuple[int, int, BinaryTree]] = MinHeap(
        key=lambda item: (item[0], item[1])
    )
    sequence = 0
    for character, frequency in sorted(frequencies.items()):
        heap.push((frequency, sequence, BinaryTree(frequency, character)))
        sequence += 1

    def serialize(node: BinaryTree) -> dict[str, Any]:
        return {
            "character": node.character,
            "frequency": node.frequency,
            "left": serialize(node.left) if node.left is not None else None,
            "right": serialize(node.right) if node.right is not None else None,
        }

    def snapshot(
        action: str,
        extracted: list[dict[str, Any]] | None = None,
        merged: list[str] | None = None,
        tree: BinaryTree | None = None,
        phase: str = "merge",
    ) -> dict[str, Any]:
        return {
            "action": action,
            "phase": phase,
            "merged": merged or [],
            "extracted": extracted or [],
            "tree": serialize(tree) if tree is not None else None,
            "nodes": [
                {
                    "character": node.character,
                    "frequency": frequency,
                    "tree": serialize(node),
                }
                for frequency, _, node in sorted(heap._items, key=lambda item: (item[0], item[1]))
            ],
        }

    steps = [snapshot("Count frequencies")]
    while len(heap) > 1:
        smallest = sorted(heap._items, key=lambda item: (item[0], item[1]))[:2]
        selected = [
            {"character": smallest[0][2].character, "frequency": smallest[0][0]},
            {"character": smallest[1][2].character, "frequency": smallest[1][0]},
        ]
        selected.sort(key=lambda item: (item["frequency"], item["character"] or ""))
        steps.append(
            snapshot(
                f"Pick {selected[0]['frequency']} + {selected[1]['frequency']}",
                selected,
                phase="select",
            )
        )
        left_frequency, _, left = heap.pop_min()
        right_frequency, _, right = heap.pop_min()
        parent = BinaryTree(left_frequency + right_frequency, left=left, right=right)
        extracted = [
            {"character": left.character, "frequency": left_frequency, "tree": serialize(left)},
            {"character": right.character, "frequency": right_frequency, "tree": serialize(right)},
        ]
        heap.push((parent.frequency, sequence, parent))
        sequence += 1
        left_label = left.character if left.is_leaf else f"{left.frequency} node"
        right_label = right.character if right.is_leaf else f"{right.frequency} node"
        steps.append(
            snapshot(
                f"Merge {left_frequency} + {right_frequency} = {parent.frequency}",
                extracted,
                [left_label or "?", right_label or "?"],
                parent,
            )
        )
    return steps


def generate_codes(root: BinaryTree | None) -> dict[str, str]:
    codes: dict[str, str] = {}
    if root is None:
        return codes

    def visit(node: BinaryTree, prefix: str) -> None:
        if node.is_leaf:
            # A one-character alphabet still needs one decodable bit per symbol.
            codes[node.character or ""] = prefix or "0"
            return
        if node.left is not None:
            visit(node.left, prefix + "0")
        if node.right is not None:
            visit(node.right, prefix + "1")

    visit(root, "")
    return codes


def encode(text: str, codes: dict[str, str]) -> str:
    return "".join(codes[character] for character in text)


def decode(bits: str, root: BinaryTree | None) -> str:
    if root is None:
        return ""
    if root.is_leaf:
        return root.character * len(bits)

    characters: list[str] = []
    node = root
    for bit in bits:
        node = node.left if bit == "0" else node.right
        if node is None:
            raise ValueError("encoded data does not match the Huffman tree")
        if node.is_leaf:
            characters.append(node.character or "")
            node = root
    if node is not root:
        raise ValueError("encoded data ends in the middle of a Huffman code")
    return "".join(characters)


def compress_text(text: str) -> dict[str, Any]:
    frequencies = dict(
        sorted(Counter(text).items(), key=lambda item: (-item[1], item[0]))
    )
    tree = build_huffman_tree(frequencies)
    codes = generate_codes(tree)
    encoded = encode(text, codes)
    decoded = decode(encoded, tree)

    original_bytes = text.encode("utf-8")
    original_binary = " ".join(f"{byte:08b}" for byte in original_bytes)
    encoded_binary_grouped = " ".join(
        encoded[index:index + 8] for index in range(0, len(encoded), 8)
    )
    original_bits = len(original_bytes) * 8
    compressed_bits = len(encoded)
    ratio = (compressed_bits / original_bits) if original_bits else 0.0
    return {
        "original_text": text,
        "frequency_map": frequencies,
        "codes": codes,
        "encoded_binary": encoded,
        "encoded_binary_grouped": encoded_binary_grouped,
        "build_steps": build_huffman_steps(frequencies),
        "decoded_text": decoded,
        "original_binary": original_binary,
        "compression_ratio": round(ratio, 4),
        "original_length": len(text),
        "compressed_length": compressed_bits,
        "original_bits": original_bits,
        "compressed_bits": compressed_bits,
    }


class CompressionHandler(BaseHTTPRequestHandler):
    """Serve the frontend and the compression API."""

    def _send_json(self, payload: dict[str, Any], status: HTTPStatus) -> None:
        body = json.dumps(payload, ensure_ascii=False).encode("utf-8")
        self.send_response(status)
        self.send_header("Content-Type", "application/json; charset=utf-8")
        self.send_header("Content-Length", str(len(body)))
        self.send_header("Access-Control-Allow-Origin", "*")
        self.end_headers()
        self.wfile.write(body)

    def do_OPTIONS(self) -> None:
        self.send_response(HTTPStatus.NO_CONTENT)
        self.send_header("Access-Control-Allow-Origin", "*")
        self.send_header("Access-Control-Allow-Headers", "Content-Type")
        self.send_header("Access-Control-Allow-Methods", "POST, OPTIONS")
        self.end_headers()

    def do_GET(self) -> None:
        path = urlparse(self.path).path
        if path not in {"/", "/learn"}:
            self._send_json({"error": "Not found"}, HTTPStatus.NOT_FOUND)
            return
        filename = "learn.html" if path == "/learn" else "index.html"
        frontend_path = os.path.join(os.path.dirname(__file__), "templates", filename)
        try:
            with open(frontend_path, "rb") as frontend:
                body = frontend.read()
        except OSError:
            self._send_json({"error": "Frontend file is unavailable"}, HTTPStatus.NOT_FOUND)
            return
        self.send_response(HTTPStatus.OK)
        self.send_header("Content-Type", "text/html; charset=utf-8")
        self.send_header("Content-Length", str(len(body)))
        self.end_headers()
        self.wfile.write(body)

    def do_POST(self) -> None:
        if urlparse(self.path).path != "/api/compress":
            self._send_json({"error": "Not found"}, HTTPStatus.NOT_FOUND)
            return
        try:
            length = int(self.headers.get("Content-Length", "0"))
            request = json.loads(self.rfile.read(length))
            text = request.get("text") if isinstance(request, dict) else None
            if not isinstance(text, str):
                raise ValueError("The request must contain a string field named 'text'.")
            self._send_json(compress_text(text), HTTPStatus.OK)
        except (ValueError, TypeError, json.JSONDecodeError) as error:
            self._send_json({"error": str(error)}, HTTPStatus.BAD_REQUEST)

    def log_message(self, format: str, *args: Any) -> None:
        print(f"[http] {self.address_string()} - {format % args}")


def run_server() -> None:
    port = int(os.environ.get("PORT", 8000))
    server = ThreadingHTTPServer(("0.0.0.0", port), CompressionHandler)
    print(f"Huffman Text Compression running on port {port}")
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        print("\nStopping server.")
    finally:
        server.server_close()


if __name__ == "__main__":
    run_server()
