# Huffman Text Compression

An interactive Huffman coding project for Advanced Data Structures at Chandigarh University.

## Features

- Custom binary `MinHeap` implementation without `heapq`
- Huffman `BinaryTree` construction using the greedy algorithm
- Lossless encoding and decoding verification
- Compression statistics and binary visualizations
- Interactive tree-building lesson at `/learn`
- Standard-library-only Python backend
- Vanilla HTML, CSS, and JavaScript frontend

## Run locally

```bash
python3 app.py
```

Open [http://localhost:8000](http://localhost:8000).

Learning page: [http://localhost:8000/learn](http://localhost:8000/learn)

## API

`POST /api/compress`

```json
{"text": "BANANA BANDANA"}
```

The response includes frequencies, Huffman codes, encoded bits, decoded text, compression measurements, and animation step data.

## Project report

See [Huffman_Text_Compression_Project_Report.docx](./Huffman_Text_Compression_Project_Report.docx) for the academic report.
