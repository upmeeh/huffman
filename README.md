# Campus Navigation and Shortest Path Finder

A beginner-friendly **C++17** project that models a university campus as a weighted, undirected graph and finds shortest walking routes between buildings using **Dijkstra's Algorithm**.

---

## Project Goals

| Goal | Detail |
|---|---|
| Data Structure | Weighted, undirected graph using an **adjacency list** |
| Algorithm | **Dijkstra's** shortest-path algorithm with a min-priority queue |
| Language | C++17 – no external libraries |
| Design | Modular: graph logic is separated from path-finding logic |

---

## File Structure

```
adsa/
├── main.cpp          # Entry point – loads data, runs demos, interactive loop
├── Graph.h           # Graph class declaration
├── Graph.cpp         # Graph class implementation (adjacency list, file loader)
├── Dijkstra.h        # Dijkstra solver & result struct declaration
├── Dijkstra.cpp      # Dijkstra implementation (min-heap, path reconstruction)
├── campus_data.txt   # Campus map: edges in "NodeA NodeB Weight" format
└── README.md         # This file
```

---

## How to Build

### Prerequisites
- **g++ 6.1+** (supports C++17) — MinGW on Windows, GCC on Linux/macOS
- No third-party libraries required

### Compile (single command)

```bash
g++ -std=c++17 -Wall -Wextra -o campus_nav main.cpp Graph.cpp Dijkstra.cpp
```

### Run

```bash
./campus_nav         # Linux / macOS
campus_nav.exe       # Windows
```

> The executable must be run from the same directory as `campus_data.txt`,
> or edit the `dataFile` variable in `main.cpp` with the correct path.

---

## Data File Format (`campus_data.txt`)

Each non-comment line defines one undirected edge:

```
BuildingA  BuildingB  DistanceInMetres
```

- Building names **must not contain spaces** – use underscores (`Hostel_A`).
- Distances must be **positive** (Dijkstra requires non-negative weights).
- Lines starting with `#` are comments.

### Example lines

```
MainGate    AdminBlock   150
CSE_Dept    Library      140
Hostel_A    Hostel_B     120
```

You can add or modify edges freely – the graph rebuilds on every run.

---

## How It Works

### 1. Graph Representation

The campus map is stored as an **adjacency list**:

```
Node index → vector of (neighbour_index, weight) pairs
```

An `unordered_map<string, int>` translates building names to indices so lookups are O(1).

### 2. Dijkstra's Algorithm (step by step)

```
1. dist[source] = 0;  all other dist[] = ∞
2. Push (0, source) into a min-heap
3. While heap not empty:
     (d, u) = heap.pop()          ← cheapest unprocessed node
     if d > dist[u]: skip          ← stale entry
     for each (v, w) in neighbours[u]:
         if dist[u] + w < dist[v]:
             dist[v] = dist[u] + w
             prev[v] = u           ← remember predecessor
             heap.push(dist[v], v)
4. Reconstruct path: follow prev[] from target back to source
```

**Time complexity:** O((V + E) log V) with a binary heap.

### 3. Interactive Mode

After the demo output the program enters an interactive loop:

```
Enter SOURCE building name: MainGate
Enter DESTINATION building name: Hostel_D

--- Shortest Path: MainGate --> Hostel_D ---
  Route: MainGate -> Sports_Complex -> Hostel_A -> Hostel_B -> Hostel_C -> Hostel_D
  Total Distance: 1030.0 metres
  Step-by-step:
    Step 1: MainGate     -> Sports_Complex  (400.0 m)
    Step 2: Sports_Complex -> Hostel_A      (180.0 m)
    ...
```

---

## Campus Buildings

| Node Name | Description |
|---|---|
| `MainGate` | Campus entrance |
| `AdminBlock` | Administration offices |
| `CSE_Dept` | Computer Science department |
| `MechE_Dept` | Mechanical Engineering |
| `ECE_Dept` | Electronics & Communication |
| `Library` | Central library |
| `Seminar_Hall` | Seminar and conference hall |
| `Auditorium` | Main auditorium |
| `Research_Lab` | Research and innovation lab |
| `Cafeteria` | Food court |
| `Sports_Complex` | Sports and recreation |
| `Hostel_A` – `Hostel_D` | Student hostels |
| `Medical_Centre` | Campus health centre |

---

## Extending the Project

| Idea | How |
|---|---|
| Add a new building | Add a line in `campus_data.txt` |
| Add bi-directional weights | The loader already stores both directions |
| Find all paths from one source | Call `Dijkstra::printAllDistances()` |
| Multiple algorithms | Add `BFS.h/cpp` or `AStar.h/cpp` beside `Dijkstra.h/cpp` |
| Visualise the map | Export adjacency list to DOT format for Graphviz |

---

## Key C++ Concepts Demonstrated

- `struct` for edge representation
- `class` with public/private separation
- `std::unordered_map` for O(1) name-to-index lookup
- `std::vector<std::vector<Edge>>` adjacency list
- `std::priority_queue` with custom comparator (`std::greater<>`) for min-heap
- Structured bindings (`auto [d, u] = ...`) – C++17
- File I/O with `std::ifstream` and `std::istringstream`
- Separation of concerns across header/source pairs

---

*Built as a Data Structures & Algorithms demonstration project.*

