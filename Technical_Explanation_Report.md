# Technical Explanation Report: Campus Navigation and Shortest Path Finder Using Dijkstra's Algorithm

---

## 1. PROJECT INTRODUCTION

### 1.1 Project Overview
The **Campus Navigation and Shortest Path Finder** is a console-based Data Structures and Algorithms (DSA) software application developed in modern C++. Its core mission is to represent a university campus as a weighted, undirected graph and determine the most optimal (shortest walking distance) route between any two buildings using **Dijkstra's Shortest Path Algorithm**.

Unlike industrial routing engines that incorporate complex third-party spatial databases or heavy algorithmic frameworks, this educational project is engineered with an emphasis on conceptual clarity, algorithmic purity, and structural transparency. The system relies strictly on fundamental C++ constructs: fixed-size 1D and 2D arrays, nested control loops, and direct standard stream I/O. By stripping away complex Standard Template Library (STL) abstractions (such as heap-based priority queues, dynamic vectors, and hash maps), the codebase makes every underlying algorithmic step observable, auditable, and easy to explain during academic evaluations.

### 1.2 Problem Statement
Universities, research campuses, and hospital complexes often span large geographic areas comprising multiple academic departments, residential hostels, administrative offices, libraries, auditoriums, and utility centers. Traversing these complexes presents recurring challenges:
- **Spatial Disorientation**: New students, visiting lecturers, conference delegates, and emergency responders often do not know the layout or the shortest walkable path between facilities.
- **Suboptimal Route Selection**: Individuals typically rely on habit or intuition, which often results in navigating congested perimeter roads or taking longer paths rather than direct inner walkways.
- **Path Interdependence and Connectivity**: When new pathways open or routes change, recalculating minimum travel paths manually across dozens of interconnected locations becomes error-prone and tedious.

A computational model is needed to capture campus topology and quickly calculate the exact shortest route and distance between any two locations.

### 1.3 Project Objectives
The engineering and educational objectives of this project are:
1. **Model Physical Campus Topology**: Model buildings as vertices and physical pathways as positive-weighted edges using an **Adjacency Matrix**.
2. **Implement Dijkstra's Algorithm from Fundamental Principles**: Execute single-source shortest path computations using direct iterative loops and array-based bookkeeping without STL priority queues or external libraries.
3. **Provide Full Path Reconstruction**: Not only compute the scalar total distance in meters, but also trace and reconstruct the sequence of intermediate buildings from origin to destination.
4. **Interactive Campus Management**: Provide an interactive menu system enabling users to display locations, inspect the connectivity matrix, query routes, insert new buildings, and define new paths dynamically.
5. **Academic Transparency**: Maintain an accessible codebase tailored for presentation and oral examination (viva voce) in undergraduate computer science coursework.

### 1.4 Practical Relevance of Campus Navigation
Automated pathfinding is a fundamental problem in computer science. While commercial navigation engines (like Google Maps) operate at planetary scales with multi-modal transportation layers, the underlying algorithmic foundation remains identical: representing a physical network as a graph of nodes and edges and applying shortest-path graph traversals.

Implementing a campus navigator illustrates how abstract discrete mathematical structures directly resolve tangible logistical problems:
- Minimizing walking time between back-to-back lectures.
- Assisting campus security and medical services in finding the shortest emergency response routes.
- Demonstrating the practical utility of greedy optimization in logistical planning.

### 1.5 Scope and Boundaries of the Project
To preserve focus and comply with academic presentation objectives, the project scope is defined as follows:

- **In Scope**:
  - A fixed capacity of up to 50 campus buildings.
  - Undirected, non-negative integer edge weights representing walking distances in meters.
  - In-memory adjacency matrix representation.
  - Interactive terminal/console user interface.
  - Pre-loaded topology of 10 campus landmarks and 13 bidirectional pathways.
  - Dynamic runtime addition of new locations and edges.
  - Path reconstruction via a predecessor tracking array (`parent[]`).

- **Out of Scope (Explicitly Excluded)**:
  - Real-time GPS coordinates, latitude/longitude mapping, or GIS data layers.
  - Graphical User Interfaces (GUI), web interfaces, HTML/CSS/JavaScript rendering.
  - External SQL/NoSQL databases or persistent file serialization.
  - Dynamic traffic modeling, elevation/stairs factoring, or multi-modal transit (bicycles, campus shuttles).
  - Negative edge weights or directed one-way footpaths.

---

## 2. PROBLEM DEFINITION

### 2.1 The Computational Problem
Formally, let the campus be represented as an undirected, weighted graph:

$$G = (V, E, W)$$

Where:
- $V = \{v_0, v_1, \dots, v_{n-1}\}$ is the set of $n$ vertices representing campus buildings ($n \le 50$).
- $E \subseteq V \times V$ is the set of edges representing walkable paths connecting pairs of buildings.
- $W: E \to \mathbb{R}^+$ is the weight function assigning a strictly positive scalar distance (in meters) to each path $(u, v) \in E$.

Given a user-specified source building $v_{src} \in V$ and destination building $v_{dest} \in V$, the objective is to find a path:

$$P = \langle v_{src} = p_0, p_1, p_2, \dots, p_{k-1}, p_k = v_{dest} \rangle$$

such that the sum of the constituent edge weights is minimized:

$$\text{Cost}(P) = \sum_{i=0}^{k-1} W(p_i, p_{i+1}) \quad \text{is minimized across all possible paths from } v_{src} \text{ to } v_{dest}.$$

If no sequence of connected edges joins $v_{src}$ and $v_{dest}$, the system must formally detect that the target is unreachable and alert the user without crashing or entering an infinite loop.

```
+----------+          150m          +------------+          180m          +----------+
| MainGate | ---------------------- | AdminBlock | ---------------------- | CSE_Dept |
+----------+                        +------------+                        +----------+
     |                                    |                                     |
     | 200m                               | 120m                                | 140m
     |                                    |                                     |
+-----------+         250m          +------------+          250m          +----------+
| Cafeteria | --------------------- | Auditorium | ---------------------- | Library  |
+-----------+                       +------------+                        +----------+
```

### 2.2 Why a Shortest-Path Algorithm is Required
A naive approach might attempt to enumerate all possible paths between two vertices using exhaustive depth-first search (Brute-Force Search). However, the number of simple paths in a densely connected graph can grow factorially with the number of vertices ($O(V!)$). In a campus network with cycles and cross-paths, brute-force enumeration quickly leads to combinatorial explosion.

Unweighted graph traversal algorithms such as Breadth-First Search (BFS) guarantee finding the path with the *minimum number of hops (edges)*. However, in physical geography, fewer hops do not correlate to shorter physical distances. For instance, a single direct road might be 1,500 meters long, whereas an alternative route through two intermediate walkways might measure only $300 + 400 = 700$ meters. BFS would pick the 1,500m single-hop path, which is suboptimal.

Dijkstra’s algorithm addresses this exact requirement. It evaluates cumulative edge costs, guaranteeing an optimal solution in polynomial time ($O(V^2)$ with arrays) as long as all edge weights are non-negative.

### 2.3 How the Project Solves the Problem
1. **Direct Index Mapping**: Maps arbitrary string building names (e.g., `"MainGate"`, `"Library"`) to contiguous integer identifiers $[0, n-1]$ using linear scanning over a simple string array.
2. **Matrix Initialization**: Populates a $50 \times 50$ 2D integer array with a designated infinity sentinel (`INF = 1e9`) for disconnected pairs and `0` for self-loops.
3. **Deterministic Relaxation**: Executes an array-based Dijkstra traversal that iteratively picks the unvisited vertex with the minimum tentative distance, finalizes its cost, and relaxes its adjacent edges.
4. **Predecessor Tracing**: Maintains a 1D `parent[]` array that stores the back-pointer for each node, enabling $O(V)$ path reconstruction via backward iteration followed by array reversal.

### 2.4 Expected Academic Learning Outcomes
By studying and defending this implementation, students demonstrate mastery in:
- Implementing non-linear graph data structures without third-party libraries.
- The mechanics of greedy algorithms, particularly why greedy choices lead to a globally optimal solution in non-negative weighted graphs.
- Edge relaxation and why tentative distance guarantees hold.
- Back-pointer path reconstruction using predecessor arrays.
- Memory layout trade-offs between adjacency matrices and adjacency lists.
- Designing modular, clean, and defensively checked console software in C++.

---

## 3. TECHNOLOGIES AND REQUIREMENTS

### 3.1 Programming Language
- **C++ (Standard: C++17)**
  - Utilizes standard streams (`cin`, `cout`) and formatting utilities (`iomanip`).
  - Strict type checking, value semantics, and procedural clarity.
  - Zero use of advanced template metaprogramming or dynamic memory pointers (`new`/`delete`), eliminating potential memory leaks.

### 3.2 Compiler and Development Environment
- **Compiler**: GNU Compiler Collection (`g++` / MinGW-w64).
- **Compilation Command**:
  ```bash
  g++ -std=c++17 main.cpp -o CampusNavigation.exe
  ```
- **Portability**: The code adheres to standard ANSI/ISO C++ specifications. Although developed on Windows 10/11 using MinGW, it compiles without modification on Linux distributions (via GCC/Clang) and macOS.

### 3.3 Standard Libraries Used
The code includes the header `<bits/stdc++.h>` (supported across GCC and MinGW distributions). In environments where `<bits/stdc++.h>` is not pre-packaged (such as Microsoft Visual C++), the required functionality decomposes into standard ANSI C++ headers:

| Header | Purpose in Project |
|---|---|
| `<iostream>` | Console stream input/output (`cin`, `cout`). |
| `<string>` | Handling building names (`string`), substrings, and string comparisons. |
| `<climits>` | Constant definitions (such as `INT_MAX`, though an explicit `1e9` is used for `INF`). |
| `<iomanip>` | Stream formatting manipulators like `setw()`, `left`, and `right` for tabular matrix rendering. |

No third-party libraries, graphing engines, or external packages are used.

### 3.4 Hardware and Software Requirements
- **Operating System**: Windows 7/8/10/11, Ubuntu 18.04+, or macOS 10.14+.
- **Processor**: Any standard x86, x64, or ARM architecture processor.
- **RAM**: Minimal memory footprint (< 5 MB peak working set).
- **Disk Storage**: Source file (~18 KB), compiled binary executable (~75–120 KB).

### 3.5 Justification for a Console/Terminal Architecture
A console-based interface was chosen for clear pedagogical and technical reasons:
1. **Algorithmic Focus**: Isolates data structure manipulation and algorithmic steps from the boilerplate required by GUI toolkits (Qt, Win32 API, wxWidgets) or web runtimes.
2. **Zero Dependencies**: Ensures the project compiles out-of-the-box on an examiner's computer without requiring runtime frameworks, DLL installations, or local web servers.
3. **Execution Transparency**: The terminal interface displays the raw adjacency matrix and step-by-step path logs directly, allowing faculty to immediately inspect the algorithm's execution state.

---

## 4. DATA STRUCTURES USED

The project uses only fundamental, fixed-size data structures.

```
+-----------------------------------------------------------------------------------+
|                                  GLOBAL MEMORY                                    |
|                                                                                   |
|  numLocations = 10                                                                |
|                                                                                   |
|  locations[50]                                                                    |
|  +--------------+--------------+--------------+--------------+-----+              |
|  | [0] MainGate | [1] Admin... | [2] CSE_Dept | [3] Library  | ... |              |
|  +--------------+--------------+--------------+--------------+-----+              |
|                                                                                   |
|  graph[50][50] (Adjacency Matrix)                                                 |
|        [0]    [1]    [2]    [3]   ...                                             |
|  [0]    0     150    INF    INF   ...                                             |
|  [1]   150     0     180    INF   ...                                             |
|  [2]   INF    180     0     140   ...                                             |
|  [3]   INF    INF    140     0    ...                                             |
|  ...                                                                              |
+-----------------------------------------------------------------------------------+
```

### 4.1 Graph
- **What it is**: A mathematical model composed of a finite set of vertices (nodes) and pairs of vertices called edges.
- **Why we use it**: It provides the natural abstraction for spatial networks where locations have discrete connectivity.
- **How it works**: Vertices represent entities; edges represent permitted transitions between them.
- **Where it appears**: Conceptually realized across `locations[]` and `graph[][]`.

### 4.2 Weighted Undirected Graph
- **What it is**: A graph where each edge $(u, v)$ has an associated numerical value (weight) representing distance, and traversing from $u$ to $v$ is equivalent in cost to traversing from $v$ to $u$.
- **Why we use it**: Walking paths on a campus are bidirectional (a pedestrian can walk both ways along a sidewalk) and have measurable lengths in meters.
- **Where it appears**: In `addPath()`, where bidirectional symmetry is enforced:
  ```cpp
  graph[u][v] = distance;
  graph[v][u] = distance;
  ```

### 4.3 Adjacency Matrix (`graph[50][50]`)
- **What it is**: A two-dimensional square array of dimensions $MAX \times MAX$ where the row index $i$ and column index $j$ represent vertices, and the value stored at `graph[i][j]` represents the edge weight connecting them.
- **Why we use it**: 
  - Allows $O(1)$ edge existence and edge weight queries.
  - Simplifies edge relaxation in Dijkstra's inner loop.
  - Provides a predictable memory footprint without dynamic pointer chasing.
- **Where it appears**: Line 52 of `main.cpp`:
  ```cpp
  int graph[MAX][MAX];
  ```
- **Example**: If building 0 (`"MainGate"`) is connected to building 1 (`"AdminBlock"`) by 150m, then:
  $$\text{graph}[0][1] = 150 \quad \text{and} \quad \text{graph}[1][0] = 150$$

### 4.4 Locations Array (`locations[50]`)
- **What it is**: A one-dimensional array of `std::string` objects.
- **Why we use it**: Acts as a symbol table translating between internal integer identifiers (0, 1, 2, ...) and human-readable building names (`"MainGate"`, `"Auditorium"`).
- **Where it appears**: Line 51 of `main.cpp`:
  ```cpp
  string locations[MAX];
  ```
- **Example**:
  $$\text{locations}[0] = \text{"MainGate"}, \quad \text{locations}[1] = \text{"AdminBlock"}$$

### 4.5 Distance Array (`dist[50]`)
- **What it is**: A one-dimensional integer array used during Dijkstra's algorithm.
- **Why we use it**: Tracks the current known minimum distance from the chosen source vertex to every other vertex in the graph.
- **Where it appears**: Inside `dijkstra()`, line 214 of `main.cpp`:
  ```cpp
  void dijkstra(int src, int dist[], int parent[])
  ```
- **Example**: If running from source node 0, initially $\text{dist}[0] = 0$, and for all $i \neq 0$, $\text{dist}[i] = 10^9$. If a path of length 150m to node 1 is discovered, $\text{dist}[1]$ relaxes to 150.

### 4.6 Visited Array (`visited[50]`)
- **What it is**: A one-dimensional boolean array.
- **Why we use it**: Prevents re-evaluating vertices whose shortest path has already been locked in. This guarantees algorithmic termination and prevents infinite cycling.
- **Where it appears**: Inside `dijkstra()`, line 215 of `main.cpp`:
  ```cpp
  bool visited[MAX];
  ```
- **Example**: $\text{visited}[u] = \text{true}$ indicates that the distance $\text{dist}[u]$ is provably optimal and cannot be improved further.

### 4.7 Parent Array (`parent[50]`)
- **What it is**: A one-dimensional integer array storing predecessor indices.
- **Why we use it**: Dijkstra’s algorithm natively computes only minimum scalar distances. To reconstruct the actual path of buildings, each node must remember which preceding vertex relaxed its distance.
- **Where it appears**: Inside `dijkstra()` and passed to `printPath()`.
- **Example**: If traveling to node 3 (`"Library"`) through node 2 (`"CSE_Dept"`), then:
  $$\text{parent}[3] = 2$$
  Tracing backward from destination to source reconstructs the complete route.

---

## 5. GRAPH REPRESENTATION

### 5.1 Vertices as Campus Buildings
Every unique campus building added to the system is assigned a zero-based integer index corresponding to its insertion order:

| Index | Building Name |
|:---:|:---|
| 0 | `MainGate` |
| 1 | `AdminBlock` |
| 2 | `CSE_Dept` |
| 3 | `Library` |
| 4 | `Cafeteria` |
| 5 | `MechE_Dept` |
| 6 | `ECE_Dept` |
| 7 | `Auditorium` |
| 8 | `Hostel_A` |
| 9 | `Medical_Centre` |

The global variable `numLocations` tracks the current count of registered vertices.

### 5.2 Edges as Walkways
An edge exists between building $u$ and building $v$ if and only if:
$$\text{graph}[u][v] \neq \text{INF} \quad \text{and} \quad u \neq v$$
The integer value stored at that coordinate represents the walking distance in meters.

### 5.3 The Meaning of Infinity (`INF = 1e9`)
In theoretical graph mathematics, the absence of an edge is denoted by an edge weight of $\infty$. In hardware memory, true infinity does not exist. The application defines:
```cpp
const int INF = 1e9; // 1,000,000,000
```
#### Why $10^9$ is chosen instead of `INT_MAX` (2,147,483,647):
If `INT_MAX` is used and an edge relaxation step evaluates `dist[u] + graph[u][v]`, adding any positive integer to `INT_MAX` triggers **signed integer arithmetic overflow**, wrapping the value to a negative number. This would corrupt Dijkstra's comparison logic. Setting `INF = 1e9` leaves sufficient headroom to add valid edge weights (e.g., $10^9 + 500$) without overflowing a standard 32-bit signed integer.

### 5.4 Zero on the Diagonal
For every vertex $i$:
$$\text{graph}[i][i] = 0$$
This captures the physical reality that moving from a building to itself requires zero walking distance.

### 5.5 Matrix Symmetry in Undirected Graphs
Because campus footpaths can be traversed in either direction, the adjacency matrix is **symmetric across its main diagonal**:
$$\text{graph}[i][j] = \text{graph}[j][i] \quad \forall \; i, j$$

### 5.6 Small Example Matrix
Consider a 4-building campus sub-network:

```
  (0) MainGate ------- 150m ------- (1) AdminBlock
       |                                   |
      200m                                120m
       |                                   |
  (4) Cafeteria ---------------------------+
```

The corresponding $4 \times 4$ slice of the adjacency matrix is:

| | [0] MainGate | [1] AdminBlock | [4] Cafeteria | [2] CSE_Dept |
|:---|:---:|:---:|:---:|:---:|
| **[0] MainGate** | 0 | 150 | 200 | INF |
| **[1] AdminBlock**| 150 | 0 | 120 | 180 |
| **[4] Cafeteria** | 200 | 120 | 0 | INF |
| **[2] CSE_Dept** | INF | 180 | INF | 0 |

---

## 6. DIJKSTRA'S ALGORITHM

### 6.1 Algorithmic Essence
Formulated by Dutch computer scientist Edsger W. Dijkstra in 1956, Dijkstra’s algorithm solves the **Single-Source Shortest Path (SSSP)** problem for graphs with non-negative edge weights. It is categorized as a **Greedy Algorithm** because at each iteration, it commits to the unvisited vertex that currently has the smallest tentative distance, locking it in as permanently optimal.

```
       +-------------------------------------------------------+
       |               START: Initialize Arrays                |
       | dist[src] = 0, all others = INF; visited[all] = false |
       +-------------------------------------------------------+
                                   |
                                   v
             +-------------------------------------------+
+----------> | Find unvisited vertex 'u' with MIN dist   |
|            +-------------------------------------------+
|                                  |
|               +------------------+------------------+
|               |                                     |
|               v (Found valid u)                     v (u == -1: All reachable visited)
|    +-----------------------+              +-----------------------+
|    | Mark visited[u] = true|              |   Algorithm Complete  |
|    +-----------------------+              +-----------------------+
|               |
|               v
|    +-----------------------------------------------+
|    | For each v: Check if graph[u][v] != INF       |
|    | If dist[u] + graph[u][v] < dist[v]            |
|    |   --> dist[v] = dist[u] + graph[u][v]         |
|    |   --> parent[v] = u                           |
|    +-----------------------------------------------+
|               |
+---------------+ (Repeat loop)
```

### 6.2 Step-by-Step Execution Mechanics

#### Step 1: Initialization
- An output array `dist[MAX]` is allocated; every entry is set to `INF`.
- A helper array `visited[MAX]` is initialized to `false`.
- A predecessor array `parent[MAX]` is populated with `-1`.
- The source vertex distance is set to zero:
  $$\text{dist}[src] = 0$$

#### Step 2: The Outer Iteration Loop
The algorithm runs an outer loop up to `numLocations` times. In each iteration, exactly one vertex will have its shortest path finalized.

#### Step 3: Finding the Minimum-Distance Unvisited Vertex
The algorithm executes an inner linear search over all registered locations:
```cpp
int u = -1;
int minD = INF;

for (int i = 0; i < numLocations; i++) {
    if (!visited[i] && dist[i] < minD) {
        minD = dist[i];
        u = i;
    }
}
```
If `u` remains `-1`, all remaining unvisited vertices are completely disconnected from the source; the loop terminates early.

#### Step 4: Marking as Visited
```cpp
visited[u] = true;
```
Once marked, `dist[u]` represents the absolute shortest distance from `src` to `u`.

#### Step 5: Edge Relaxation
For every potential neighbor $v \in [0, \text{numLocations}-1]$, the algorithm checks if moving through $u$ offers a shorter path:
```cpp
for (int v = 0; v < numLocations; v++) {
    if (visited[v]) continue;
    if (graph[u][v] == INF) continue;

    int newDist = dist[u] + graph[u][v];
    if (newDist < dist[v]) {
        dist[v] = newDist; // Distance updated
        parent[v] = u;     // Predecessor recorded
    }
}
```

#### Step 6: Path Reconstruction
To print the full path to a destination `dest`:
1. Start at `dest`.
2. Follow `parent[dest]`, `parent[parent[dest]]`, etc., backward until reaching `src` (where `parent[src] == -1`).
3. Store these nodes in a local array `path[]`.
4. Traverse `path[]` in reverse order to display the forward journey:
   $$src \to p_1 \to p_2 \dots \to dest$$

---

## 7. COMPLETE CODE EXPLANATION

This section walks through every function in `main.cpp` in its exact order of appearance.

### 7.1 `trim()`
```cpp
string trim(string s) {
    int start = 0;
    int end   = (int)s.size() - 1;
    while (start <= end && (s[start] == ' ' || s[start] == '\t' || s[start] == '\r'))
        start++;
    while (end >= start && (s[end] == ' ' || s[end] == '\t' || s[end] == '\r'))
        end--;
    return s.substr(start, end - start + 1);
}
```
- **Purpose**: Strips leading and trailing whitespace, tabs, and carriage return characters (`\r`) from user input strings.
- **Input**: A `string s` passed by value.
- **Output**: A new `string` containing only the trimmed characters.
- **Why It Is Needed**: When running in Windows consoles or processing scripted/piped test inputs, pressing Enter often leaves a carriage return (`\r`) in the input buffer. Without trimming, `"MainGate\r"` would fail to match `"MainGate"`, causing location lookups to fail unexpectedly.
- **Internal Logic**: Moves a `start` pointer forward past whitespace and an `end` pointer backward past whitespace, returning the substring between them.

### 7.2 `initGraph()`
```cpp
void initGraph() {
    for (int i = 0; i < MAX; i++) {
        for (int j = 0; j < MAX; j++) {
            if (i == j)
                graph[i][j] = 0;
            else
                graph[i][j] = INF;
        }
    }
}
```
- **Purpose**: Clears the adjacency matrix before data loading.
- **Input**: None (operates on global `graph[MAX][MAX]`).
- **Output**: None (`void`).
- **Internal Logic**: Runs two nested loops from 0 to `MAX - 1` (50 iterations each, 2,500 cells total). If row equals column ($i == j$), it writes `0`; otherwise, it writes `INF`.

### 7.3 `findIndex()`
```cpp
int findIndex(string name) {
    for (int i = 0; i < numLocations; i++) {
        if (locations[i] == name)
            return i;
    }
    return -1;
}
```
- **Purpose**: Translates a textual building name into its corresponding integer index.
- **Input**: `string name`.
- **Output**: Returns the matching integer index $[0, \text{numLocations}-1]$ if found; returns `-1` if not found.
- **Internal Logic**: Performs a linear search across `locations[0]` through `locations[numLocations - 1]`.

### 7.4 `addLocation()`
```cpp
void addLocation(string name) {
    if (numLocations >= MAX) {
        cout << "  [!] Location limit reached (max " << MAX << ").\n";
        return;
    }
    if (findIndex(name) != -1) {
        cout << "  [!] '" << name << "' already exists.\n";
        return;
    }
    locations[numLocations] = name;
    numLocations++;
    cout << "  [+] Location added: " << name << "\n";
}
```
- **Purpose**: Inserts a new building into the campus registry.
- **Input**: `string name`.
- **Output**: None (`void`).
- **Internal Logic**:
  1. Guard clause: Ensures `numLocations < MAX` (preventing array overflow).
  2. Duplicate check: Ensures `findIndex(name) == -1`.
  3. Writes `name` to `locations[numLocations]` and increments `numLocations`.

### 7.5 `addPath()`
```cpp
void addPath(string from, string to, int distance) {
    if (distance <= 0) {
        cout << "  [!] Distance must be a positive number.\n";
        return;
    }
    if (findIndex(from) == -1) addLocation(from);
    if (findIndex(to)   == -1) addLocation(to);

    int u = findIndex(from);
    int v = findIndex(to);

    if (u == -1 || v == -1) {
        cout << "  [!] Could not add path.\n";
        return;
    }

    graph[u][v] = distance;
    graph[v][u] = distance;

    cout << "  [+] Path: " << from << " <--> " << to
         << "  (" << distance << " m)\n";
}
```
- **Purpose**: Creates an undirected, weighted connection between two buildings.
- **Input**: `string from`, `string to`, `int distance`.
- **Output**: None (`void`).
- **Internal Logic**:
  1. Validates that `distance > 0` (Dijkstra requires non-negative weights).
  2. Auto-creates either location if it does not yet exist.
  3. Fetches the indices `u` and `v`.
  4. Writes `distance` to both `graph[u][v]` and `graph[v][u]` to reflect an undirected edge.

### 7.6 `showLocations()`
```cpp
void showLocations() {
    cout << "\n+------------------------------+\n";
    cout << "|  Campus Buildings            |\n";
    cout << "+------------------------------+\n";

    if (numLocations == 0) {
        cout << "  No locations added yet.\n";
        return;
    }

    for (int i = 0; i < numLocations; i++) {
        cout << "  [" << setw(2) << i << "]  " << locations[i] << "\n";
    }

    cout << "+------------------------------+\n";
    cout << "  Total: " << numLocations << " location(s)\n\n";
}
```
- **Purpose**: Displays the complete list of registered buildings with their integer identifiers.
- **Input**: None.
- **Output**: Formatted console output.
- **Internal Logic**: Loops through `locations[]` from `0` to `numLocations - 1`, formatting the index with `setw(2)`.

### 7.7 `showMatrix()`
```cpp
void showMatrix() {
    if (numLocations == 0) {
        cout << "  No data to display.\n";
        return;
    }
    cout << "\n=== Adjacency Matrix (distances in metres) ===\n\n";

    cout << setw(18) << " ";
    for (int j = 0; j < numLocations; j++) {
        cout << setw(8) << locations[j].substr(0, 6);
    }
    cout << "\n" << string(18 + numLocations * 8, '-') << "\n";

    for (int i = 0; i < numLocations; i++) {
        cout << setw(15) << left << locations[i].substr(0, 15)
             << right << " |  ";

        for (int j = 0; j < numLocations; j++) {
            if (graph[i][j] == INF)
                cout << setw(7) << "---";
            else
                cout << setw(7) << graph[i][j];
        }
        cout << "\n";
    }
    cout << string(18 + numLocations * 8, '=') << "\n\n";
}
```
- **Purpose**: Visualizes the graph's internal adjacency matrix as a formatted table.
- **Input**: None.
- **Output**: Console table.
- **Internal Logic**: Prints column headers using the first 6 characters of each building name (`substr(0, 6)`), then prints each row labeled with the building name. Cells with `INF` display as `---`.

### 7.8 `dijkstra()`
```cpp
void dijkstra(int src, int dist[], int parent[]) {
    bool visited[MAX];

    for (int i = 0; i < numLocations; i++) {
        dist[i]    = INF;
        visited[i] = false;
        parent[i]  = -1;
    }
    dist[src] = 0;

    for (int count = 0; count < numLocations; count++) {
        int u    = -1;
        int minD = INF;

        for (int i = 0; i < numLocations; i++) {
            if (!visited[i] && dist[i] < minD) {
                minD = dist[i];
                u    = i;
            }
        }

        if (u == -1) break;

        visited[u] = true;

        for (int v = 0; v < numLocations; v++) {
            if (visited[v]) continue;
            if (graph[u][v] == INF) continue;

            int newDist = dist[u] + graph[u][v];
            if (newDist < dist[v]) {
                dist[v]   = newDist;
                parent[v] = u;
            }
        }
    }
}
```
- **Purpose**: Computes single-source shortest paths from `src` to all other nodes.
- **Input**: `int src`, output buffers `int dist[]` and `int parent[]`.
- **Output**: None directly (mutates `dist` and `parent` arrays in place).
- **Internal Logic**: Follows the six steps detailed in Section 6.

### 7.9 `printPath()`
```cpp
void printPath(int src, int dest, int dist[], int parent[]) {
    // [Header formatting omitted for brevity]
    if (dist[dest] == INF) {
        cout << "  [!] No path exists between these two locations.\n";
        return;
    }

    int path[MAX];
    int len = 0;
    int cur = dest;

    while (cur != -1) {
        path[len] = cur;
        len++;
        cur = parent[cur];
    }

    cout << "  Route: ";
    for (int i = len - 1; i >= 0; i--) {
        cout << locations[path[i]];
        if (i > 0) cout << " -> ";
    }
    cout << "\n\n";

    cout << "  Step-by-step:\n";
    for (int i = len - 1; i > 0; i--) {
        int from = path[i];
        int to   = path[i - 1];
        cout << "    Step " << (len - i) << ": "
             << setw(18) << left  << locations[from]
             << " ->  "
             << setw(18) << left  << locations[to]
             << right << "(" << graph[from][to] << " m)\n";
    }

    cout << "\n  Total Distance : " << dist[dest] << " metres\n";
}
```
- **Purpose**: Displays the shortest path route, turn-by-turn breakdown, and total distance.
- **Input**: `src`, `dest`, `dist[]`, `parent[]`.
- **Output**: Formatted console text.
- **Internal Logic**: If `dist[dest] == INF`, reports unreachable. Otherwise, back-traces predecessors into `path[]`, prints the forward sequence, and calculates individual step distances using `graph[from][to]`.

### 7.10 `findShortestRoute()`
```cpp
void findShortestRoute(string from, string to) {
    int src  = findIndex(from);
    int dest = findIndex(to);

    if (src == -1 || dest == -1) { /* error messaging */ return; }
    if (src == dest) { /* alert identical */ return; }

    int dist[MAX];
    int parent[MAX];

    dijkstra(src, dist, parent);
    printPath(src, dest, dist, parent);
}
```
- **Purpose**: High-level orchestrator that validates user input, invokes `dijkstra()`, and triggers `printPath()`.
- **Input**: Origin name `from`, destination name `to`.

### 7.11 `loadDefaultData()`
```cpp
void loadDefaultData() {
    addLocation("MainGate");
    addLocation("AdminBlock");
    // ... registers 10 locations ...
    addPath("MainGate", "AdminBlock", 150);
    // ... registers 13 paths ...
}
```
- **Purpose**: Bootstraps the application with an initial campus map so users can test pathfinding immediately on startup.

### 7.12 `showMenu()`
- **Purpose**: Prints the 6 interactive choices to the console.

### 7.13 `main()`
- **Purpose**: Application entry point.
- **Execution Flow**:
  1. Calls `initGraph()`.
  2. Calls `loadDefaultData()`.
  3. Enters an infinite `while (true)` loop:
     - Displays menu and reads `choice`.
     - Validates numeric input (clears stream on non-numeric entry via `cin.fail()`).
     - Dispatches commands 1 through 6 via an `if-else if` ladder.

---

## 8. ALGORITHM DRY RUN

To understand how Dijkstra’s algorithm works step by step, let us trace a dry run on a small 4-building sub-graph:

```
  [0] MainGate -------- 150m -------- [1] AdminBlock
       |                                   |
      200m                                120m
       |                                   |
  [4] Cafeteria ---------------------------+
```
- Nodes: $V = \{0: \text{MainGate}, 1: \text{AdminBlock}, 4: \text{Cafeteria}\}$
- Source node: **0 (`MainGate`)**
- Target destination: **4 (`Cafeteria`)**

```
Edges:
(0, 1) = 150
(0, 4) = 200
(1, 4) = 120
```

### Initial State (Before Loop Starts)
- `dist` = `[0: 0, 1: INF, 4: INF]`
- `visited` = `[0: false, 1: false, 4: false]`
- `parent` = `[0: -1, 1: -1, 4: -1]`

---

### Iteration 1
1. **Find minimum unvisited node**:
   - Compare unvisited:
     - Node 0: $\text{dist}[0] = 0$
     - Node 1: $\text{dist}[1] = \text{INF}$
     - Node 4: $\text{dist}[4] = \text{INF}$
   - Smallest is **Node 0** ($\text{minD} = 0$).
2. **Mark visited**:
   - `visited[0] = true`.
3. **Relax neighbors of Node 0**:
   - Check Node 1: Edge $(0, 1) = 150$.
     $$\text{newDist} = \text{dist}[0] + 150 = 0 + 150 = 150 < \text{dist}[1] \; (\text{INF})$$
     $\to \text{dist}[1] = 150$, $\text{parent}[1] = 0$.
   - Check Node 4: Edge $(0, 4) = 200$.
     $$\text{newDist} = \text{dist}[0] + 200 = 0 + 200 = 200 < \text{dist}[4] \; (\text{INF})$$
     $\to \text{dist}[4] = 200$, $\text{parent}[4] = 0$.

#### State after Iteration 1:
| Array | Node 0 (`MainGate`) | Node 1 (`AdminBlock`) | Node 4 (`Cafeteria`) |
|---|:---:|:---:|:---:|
| `visited` | **true** | false | false |
| `dist` | **0** | **150** | **200** |
| `parent` | -1 | **0** | **0** |

---

### Iteration 2
1. **Find minimum unvisited node**:
   - Candidates (unvisited only):
     - Node 1: $\text{dist}[1] = 150$
     - Node 4: $\text{dist}[4] = 200$
   - Smallest is **Node 1** ($\text{minD} = 150$).
2. **Mark visited**:
   - `visited[1] = true`.
3. **Relax neighbors of Node 1**:
   - Check Node 0: Already visited (`visited[0] == true`), skipped.
   - Check Node 4: Edge $(1, 4) = 120$.
     $$\text{newDist} = \text{dist}[1] + 120 = 150 + 120 = 270$$
     Is $270 < \text{dist}[4]$ (which is $200$)? **No ($270 \not< 200$)**.
     $\to \text{dist}[4]$ remains $200$, $\text{parent}[4]$ remains $0$.

#### State after Iteration 2:
| Array | Node 0 (`MainGate`) | Node 1 (`AdminBlock`) | Node 4 (`Cafeteria`) |
|---|:---:|:---:|:---:|
| `visited` | true | **true** | false |
| `dist` | 0 | 150 | **200** |
| `parent` | -1 | 0 | **0** |

---

### Iteration 3
1. **Find minimum unvisited node**:
   - Only Node 4 remains unvisited: $\text{dist}[4] = 200$.
2. **Mark visited**:
   - `visited[4] = true`.
3. **Relax neighbors of Node 4**:
   - All neighbors (0 and 1) are already marked visited. No updates occur.

#### Final Algorithm State:
All reachable vertices have been visited.

### Path Reconstruction to Node 4:
- Target: 4.
- `parent[4]` = 0.
- `parent[0]` = -1 (Stop).
- Backwards path: $4 \leftarrow 0$.
- Reverse forward path: **`MainGate -> Cafeteria`**.
- Final shortest distance: **200 meters**.

*(Note: Although walking through `AdminBlock` connects to `Cafeteria`, the total distance via that route is $150 + 120 = 270\text{m}$, which the algorithm evaluated and correctly rejected in favor of the direct 200m path).*

---

## 9. SAMPLE CAMPUS DATA

### 9.1 Predefined Locations
The system registers 10 default buildings in `loadDefaultData()`:

| Index | Location Identifier | Role on Campus |
|:---:|:---|:---|
| 0 | `MainGate` | Campus entrance and security checkpoint |
| 1 | `AdminBlock` | Central administration and registrar offices |
| 2 | `CSE_Dept` | Computer Science & Engineering department |
| 3 | `Library` | Central library and reading halls |
| 4 | `Cafeteria` | Student dining and commons facility |
| 5 | `MechE_Dept` | Mechanical Engineering workshops |
| 6 | `ECE_Dept` | Electronics & Communication labs |
| 7 | `Auditorium` | Main event hall and ceremonial theater |
| 8 | `Hostel_A` | Student residential block |
| 9 | `Medical_Centre` | Campus healthcare clinic and infirmary |

### 9.2 Predefined Walkways (Edges)

| Edge # | Location A | Location B | Distance (Meters) | Description |
|:---:|:---|:---|:---:|:---|
| 1 | `MainGate` | `AdminBlock` | 150 | Main avenue entrance |
| 2 | `MainGate` | `Cafeteria` | 200 | Direct western pedestrian lane |
| 3 | `AdminBlock` | `CSE_Dept` | 180 | Northern academic corridor |
| 4 | `AdminBlock` | `MechE_Dept` | 220 | Eastern engineering lane |
| 5 | `AdminBlock` | `Cafeteria` | 120 | Administrative lunch corridor |
| 6 | `CSE_Dept` | `Library` | 140 | Academic study walkway |
| 7 | `CSE_Dept` | `ECE_Dept` | 200 | Computing to electronics link |
| 8 | `CSE_Dept` | `MechE_Dept` | 160 | Inter-departmental pathway |
| 9 | `Library` | `Auditorium` | 250 | Library-auditorium link |
| 10 | `MechE_Dept` | `ECE_Dept` | 130 | Workshop to hardware lab |
| 11 | `Cafeteria` | `Hostel_A` | 300 | Residential dining walk |
| 12 | `Cafeteria` | `Medical_Centre` | 350 | Health center access lane |
| 13 | `Hostel_A` | `Medical_Centre` | 200 | Direct residential clinic path |

### 9.3 Alternative Routes in the Sample Topology
The sample topology creates multiple paths between distant nodes, illustrating how Dijkstra avoids suboptimal routes:

```
Route Option 1:
MainGate -> AdminBlock -> CSE_Dept -> Library -> Auditorium
Total: 150 + 180 + 140 + 250 = 720 meters

Alternative via MechE/ECE:
MainGate -> AdminBlock -> MechE_Dept -> ECE_Dept -> CSE_Dept -> Library -> Auditorium
Total: 150 + 220 + 130 + 200 + 140 + 250 = 1,090 meters
```
The algorithm evaluates these alternatives and selects the 720m route.

---

## 10. APPLICATION FEATURES

The application presents an interactive 6-option console menu:

```
========================================
   Campus Navigation  -  Main Menu
========================================
  1. Show all campus locations
  2. Show campus map (adjacency matrix)
  3. Find shortest route
  4. Add a new location
  5. Add a path between two locations
  6. Exit
========================================
```

### Option 1: Show All Campus Locations
- **Action**: Iterates over `locations[0]` through `locations[numLocations - 1]`.
- **Output**: Displays a numbered list showing each building's assigned index and name.
- **Internal Behavior**: Read-only loop ($O(V)$ time).

### Option 2: Show Campus Map (Adjacency Matrix)
- **Action**: Generates a 2D table representing the current connectivity matrix.
- **Output**: Displays cell values as numbers (meters) or `---` (no direct path).
- **Internal Behavior**: Nested loops over active nodes ($O(V^2)$ time).

### Option 3: Find Shortest Route
- **Action**: Prompts for origin and destination names.
- **Internal Behavior**:
  1. Trims inputs using `trim()`.
  2. Resolves string names to indices using `findIndex()`.
  3. Validates that both endpoints exist and are distinct.
  4. Allocates local `dist[50]` and `parent[50]` arrays.
  5. Runs `dijkstra()`.
  6. Reconstructs and prints the path via `printPath()`.

### Option 4: Add a New Location
- **Action**: Prompts for a new location name.
- **Internal Behavior**:
  1. Trims input.
  2. Verifies `numLocations < 50`.
  3. Verifies name is non-empty and not a duplicate.
  4. Appends name to `locations[numLocations]` and increments `numLocations`.

### Option 5: Add a Path Between Two Locations
- **Action**: Prompts for two building names and an integer distance.
- **Internal Behavior**:
  1. Validates that distance is positive.
  2. Auto-creates either building if it does not yet exist.
  3. Sets symmetric weights:
     ```cpp
     graph[u][v] = distance;
     graph[v][u] = distance;
     ```

### Option 6: Exit
- **Action**: Displays a goodbye message and breaks the `while (true)` loop, terminating the process with exit code 0.

---

## 11. TIME AND SPACE COMPLEXITY

### 11.1 Time Complexity Analysis

| Operation | Function in Code | Time Complexity | Justification |
|---|---|:---:|---|
| Location Lookup | `findIndex()` | $O(V)$ | Linear search through `locations[0 ... numLocations-1]`. |
| Add Location | `addLocation()` | $O(V)$ | Calls `findIndex()` for duplicate detection, then does an $O(1)$ write. |
| Add Path | `addPath()` | $O(V)$ | Two $O(V)$ location lookups followed by $O(1)$ matrix assignments. |
| Display Matrix | `showMatrix()` | $O(V^2)$ | Two nested loops iterating across `numLocations` rows and columns. |
| **Dijkstra Algorithm** | `dijkstra()` | $\mathbf{O(V^2)}$ | Outer loop runs $V$ times; each iteration does an $O(V)$ scan for min distance plus an $O(V)$ edge relaxation sweep. Total: $V \times (V + V) = 2V^2 \to O(V^2)$. |
| Path Reconstruction | `printPath()` | $O(V)$ | Traces predecessor chain backward; a simple path contains at most $V-1$ edges. |

#### Why $O(V^2)$ is Well-Suited for this Implementation:
In dense graphs where the number of edges approaches the maximum ($E \approx V^2$), an array-based Dijkstra implementation running in $O(V^2)$ is asymptotically optimal and rivals a min-heap implementation ($O((V + E) \log V)$). For a campus network bounded at $V \le 50$, $V^2 = 2,500$ operations execute in microseconds on modern processors, making the code fast while remaining simple to understand.

### 11.2 Space Complexity Analysis

| Component | Storage Size | Space Complexity | Explanation |
|---|---|:---:|---|
| Adjacency Matrix | `graph[50][50]` | $O(MAX^2)$ | A fixed $50 \times 50$ 2D array of 4-byte integers = 10,000 bytes (~10 KB). |
| Location Strings | `locations[50]` | $O(MAX)$ | 50 string objects holding building names (~2 KB). |
| Dijkstra Buffers | `dist[]`, `visited[]`, `parent[]` | $O(MAX)$ | Two integer arrays and one boolean array allocated on the stack (~450 bytes). |
| **Total Memory** | — | $\mathbf{O(MAX^2)}$ | Total data segment footprint is under **20 Kilobytes**, well within embedded and microcontroller limits. |

---

## 12. TEST CASES AND EXPECTED OUTPUTS

### Test Case 1: Standard Multi-Hop Route Finding
- **Source**: `MainGate`
- **Destination**: `Auditorium`
- **Expected Route**: `MainGate -> AdminBlock -> CSE_Dept -> Library -> Auditorium`
- **Step Breakdown**:
  - Step 1: `MainGate` $\to$ `AdminBlock` (150 m)
  - Step 2: `AdminBlock` $\to$ `CSE_Dept` (180 m)
  - Step 3: `CSE_Dept` $\to$ `Library` (140 m)
  - Step 4: `Library` $\to$ `Auditorium` (250 m)
- **Expected Total Distance**: **720 metres**

### Test Case 2: Short Route with Parallel Choices
- **Source**: `MainGate`
- **Destination**: `Hostel_A`
- **Possible Paths**:
  - Path 1: `MainGate -> Cafeteria -> Hostel_A` ($200 + 300 = 500\text{ m}$)
  - Path 2: `MainGate -> AdminBlock -> Cafeteria -> Hostel_A` ($150 + 120 + 300 = 570\text{ m}$)
- **Expected Selection**: Path 1 (`MainGate -> Cafeteria -> Hostel_A`)
- **Expected Total Distance**: **500 metres**

### Test Case 3: Same Source and Destination
- **Input**: Source: `ECE_Dept`, Destination: `ECE_Dept`
- **Expected Output**:
  ```
  [!] Source and destination are the same.
  ```

### Test Case 4: Non-Existent Building Query
- **Input**: Source: `SpaceStation`, Destination: `Library`
- **Expected Output**:
  ```
  [!] Location not found: "SpaceStation"
  ```

### Test Case 5: Dynamic Building and Path Addition
- **Input**:
  - Option 4 $\to$ Add Location: `SportsCourt`
  - Option 5 $\to$ Add Path: `SportsCourt` to `Hostel_A`, distance: `150`
  - Option 3 $\to$ Query Route: `SportsCourt` to `Library`
- **Expected Route**: `SportsCourt -> Hostel_A -> Cafeteria -> AdminBlock -> CSE_Dept -> Library`
- **Expected Total Distance**: $150 + 300 + 120 + 180 + 140 =$ **890 metres**

### Test Case 6: Completely Disconnected Building
- **Action**: Add node `IsolatedTower` with no paths attached.
- **Input**: Source: `MainGate`, Destination: `IsolatedTower`
- **Expected Output**:
  ```
  [!] No path exists between these two locations.
  ```

### Test Case 7: Invalid Distance Input
- **Input**: Path between `MainGate` and `Library` with distance `-50`
- **Expected Output**:
  ```
  [!] Distance must be a positive number.
  ```

### Test Case 8: Invalid Menu Option Handling
- **Input**: `9` (or non-numeric text like `abc`)
- **Expected Output**:
  - For numeric out-of-range:
    ```
    [!] Invalid choice. Enter a number from 1 to 6.
    ```
  - For text input: The stream error flag is cleared (`cin.clear()`), the line discarded (`cin.ignore()`), and the user prompted again without crashing.

---

## 13. ADVANTAGES AND LIMITATIONS

### 13.1 Advantages of the Current Implementation
1. **High Academic Transparency**: Pure arrays and basic loops make every step of graph traversal easy to follow and explain in an oral exam.
2. **Deterministic $O(1)$ Edge Queries**: Checking whether an edge exists between building $u$ and building $v$ takes a single array access (`graph[u][v] != INF`), avoiding pointer dereferences.
3. **Zero Dynamic Memory Hazards**: Avoiding manual heap allocation (`new`/`delete`) eliminates memory leaks, double-free bugs, and segmentation faults.
4. **Predictable Memory Footprint**: The entire graph state occupies a static memory block (< 20 KB), allowing execution on memory-constrained systems.
5. **No External Dependencies**: Compiles cleanly with standard toolchains without needing external library configurations.

### 13.2 Limitations of the Current Implementation
1. **Fixed Capacity ($MAX = 50$)**: The campus cannot exceed 50 buildings without modifying the constant and recompiling.
2. **Dense Matrix Memory Inefficiency ($O(V^2)$)**: For sparse graphs with few paths, most matrix cells store `INF`. An adjacency list would use less memory when scaling to thousands of nodes.
3. **No Support for Negative Weights**: Dijkstra's algorithm assumes edge weights are non-negative. If negative weights were introduced, the greedy choice property would break, requiring the Bellman-Ford algorithm instead.
4. **In-Memory Volatility**: New buildings and paths added during a session are stored only in RAM and reset when the program exits.
5. **Console-Only Interface**: Provides text-based directions rather than an interactive graphical map.

---

## 14. FUTURE SCOPE

While the current architecture meets its academic objectives, it can be extended in several directions:

```
+---------------------------------------------------------------------------------+
|                                 FUTURE ROADMAP                                  |
|                                                                                 |
|   +-----------------------+     +-----------------------+     +-------------+   |
|   | Persistent Storage    | --> | Graphical Front-End   | --> | Real-Time   |   |
|   | SQLite / File I/O     |     | Web / Desktop UI      |     | Tracking    |   |
|   +-----------------------+     +-----------------------+     +-------------+   |
|                                                                                 |
|   - Save campus data to         - SVG / Canvas map            - Mobile GPS      |
|     JSON/SQLite                   rendering                   - Turn-by-turn    |
|   - Dynamic editing without     - Click-to-select               voice guidance  |
|     code changes                  endpoints                   - Live congestion |
+---------------------------------------------------------------------------------+
```

### 1. Persistent Storage (File I/O or SQLite)
- Save and load campus configurations to disk (via JSON, CSV, or SQLite).
- Preserve user-added locations and paths across application restarts.

### 2. Graphical User Interface (GUI)
- Build an interactive visual interface using lightweight GUI toolkits (e.g., Dear ImGui, SFML, or a web front-end via WebAssembly).
- Render campus maps as 2D vector layouts where buildings appear as clickable nodes and paths as highlighted lines.

### 3. Real-World GIS and GPS Integration
- Associate latitude and longitude coordinates with each building.
- Calculate real-world surface walking distances via the Haversine formula or spatial map layers.

### 4. Dynamic Path Conditions (Live Routing)
- Support variable edge weights representing temporary construction closures, rainy-weather indoor paths, or crowded class-change intervals.

### 5. Multi-Criteria Pathfinding
- Allow users to optimize for different goals:
  - Fewest stairs (wheelchair-accessible routing).
  - Covered/indoor paths during rain.
  - Scenic or well-lit routes at night.

---

## 15. CONCLUSION

The **Campus Navigation and Shortest Path Finder** demonstrates how core Data Structures and Algorithms principles solve practical logistical problems. By representing a physical campus as an adjacency matrix and using Dijkstra's algorithm with simple arrays and loops, the project offers a clean, reliable, and educational implementation.

Key outcomes achieved:
1. **Algorithmic Mastery**: Demonstrates how greedy vertex selection and edge relaxation produce globally optimal shortest paths.
2. **Clear Architecture**: Plain arrays make every state change observable, providing an accessible reference for academic discussions and code defense.
3. **Robust Design**: Input trimming, array bounds checking, and cycle prevention ensure reliable console execution.

---

## 16. VIVA PREPARATION (25 FACULTY QUESTIONS & DETAILED ANSWERS)

### Q1: What is a Graph in Data Structures?
**Answer**: A graph is a non-linear data structure consisting of a set of vertices (or nodes, representing entities) and a set of edges (representing relationships or links between pairs of vertices). Formally, $G = (V, E)$.

### Q2: What is the difference between a directed and an undirected graph?
**Answer**: In a directed graph (digraph), edges have a specific direction: the edge $(u, v)$ permits travel from $u$ to $v$, but not from $v$ to $u$ unless explicitly defined. In an undirected graph, edges are bidirectional: an edge between $u$ and $v$ allows travel in both directions equally. Our campus paths are modeled as an undirected graph.

### Q3: What is a weighted graph? What do the weights represent here?
**Answer**: A weighted graph assigns a numerical value (weight) to each edge. In this project, weights represent physical walking distances between buildings measured in meters.

### Q4: Why did you choose an Adjacency Matrix over an Adjacency List?
**Answer**: For this project, an adjacency matrix (`graph[50][50]`):
1. Keeps the implementation simple by using plain 2D arrays rather than pointers, dynamic vectors, or linked lists.
2. Enables $O(1)$ edge lookups and edge updates.
3. Makes code defense straightforward during academic evaluations.
For a small network ($V \le 50$), the memory overhead is negligible (~10 KB).

### Q5: What is the time complexity of Dijkstra's Algorithm in your code?
**Answer**: $\mathbf{O(V^2)}$, where $V$ is the number of vertices. The outer loop runs $V$ times. Inside, finding the unvisited vertex with the minimum tentative distance requires scanning the `dist` array ($O(V)$), and relaxing outgoing edges checks all potential neighbors ($O(V)$). Thus, $V \times (V + V) = O(V^2)$.

### Q6: Can Dijkstra's algorithm be optimized to run faster than $O(V^2)$?
**Answer**: Yes. Using an adjacency list and a min-priority queue (such as a binary min-heap), the time complexity improves to $O((V + E) \log V)$. Using a Fibonacci heap, it can achieve $O(E + V \log V)$. However, for small graphs ($V = 50$), our array-based $O(V^2)$ loop executes in microseconds while remaining much simpler to explain.

### Q7: Why does Dijkstra's algorithm fail with negative edge weights?
**Answer**: Dijkstra's algorithm is greedy: once a vertex is marked `visited`, it assumes its shortest path is permanently locked in and never re-evaluates it. If a negative edge weight exists elsewhere in the graph, traveling through that negative edge could reduce the cost to an already-visited vertex, breaking Dijkstra's core invariant. Graphs with negative edge weights require the Bellman-Ford algorithm instead.

### Q8: What is "Edge Relaxation"?
**Answer**: Edge relaxation is the process of testing whether passing through a newly finalized vertex $u$ shortens the known path to an adjacent vertex $v$:
$$\text{If } \text{dist}[u] + \text{weight}(u, v) < \text{dist}[v], \quad \text{then } \text{dist}[v] = \text{dist}[u] + \text{weight}(u, v)$$
If true, the tentative distance is updated and $u$ is recorded as $v$'s predecessor (`parent[v] = u`).

### Q9: Why is Dijkstra's algorithm classified as a "Greedy Algorithm"?
**Answer**: Because at every iteration, it makes the locally optimal choice—selecting the unvisited vertex with the smallest tentative distance—without backtracking. For non-negative edge weights, this greedy choice is guaranteed to lead to a globally optimal solution.

### Q10: What is the purpose of the `parent[]` array?
**Answer**: The `dist[]` array stores only scalar distance values, not the sequence of visited nodes. The `parent[]` array records which vertex relaxed each node's shortest path. Tracing backward from destination to source reconstructs the complete route.

### Q11: How do you handle disconnected components or unreachable buildings?
**Answer**: Disconnected nodes maintain a distance of `INF` ($10^9$). In `dijkstra()`, if the smallest tentative distance among unvisited nodes is `INF`, the loop terminates early. In `printPath()`, if `dist[dest] == INF`, the system reports that no connecting path exists.

### Q12: Why did you use `1e9` for `INF` instead of `INT_MAX`?
**Answer**: To prevent signed integer overflow. If `INT_MAX` ($2^{31}-1 \approx 2.14 \times 10^9$) is used, evaluating `dist[u] + graph[u][v]` during relaxation would overflow into negative numbers, corrupting distance comparisons. `1e9` leaves sufficient headroom for valid additions while still acting as an effective infinity.

### Q13: What does the main diagonal in the adjacency matrix represent?
**Answer**: It represents self-loops: traveling from building $i$ to itself. These cells are initialized to `0` because moving within the same building requires zero walking distance.

### Q14: How does `addPath()` guarantee the graph is undirected?
**Answer**: By writing the distance symmetrically to both cells:
```cpp
graph[u][v] = distance;
graph[v][u] = distance;
```

### Q15: How does your program handle invalid string inputs like `"MainGate\r"`?
**Answer**: It uses a helper function `trim()` that removes leading and trailing whitespace, tabs, and carriage return characters (`\r`). This prevents lookup failures caused by trailing line-ending artifacts in terminal input buffers.

### Q16: How is a new location added at runtime?
**Answer**: `addLocation()` checks that `numLocations < MAX` and verifies the name is not a duplicate. It then stores the name in `locations[numLocations]` and increments `numLocations`.

### Q17: What is the space complexity of this project?
**Answer**: $\mathbf{O(MAX^2)}$, dominated by the $50 \times 50$ adjacency matrix of integers (10,000 bytes). Total data memory is under 20 KB.

### Q18: Could we use Breadth-First Search (BFS) for this problem?
**Answer**: BFS finds the shortest path in terms of the *minimum number of edges (hops)*, but it does not account for edge weights. In a weighted physical network where edge lengths vary, BFS can pick a long single-hop path over a much shorter two-hop route. BFS is only optimal when all edge weights are identical.

### Q19: What is the difference between Dijkstra's Algorithm and Prim's Algorithm?
**Answer**: Both algorithms use a greedy approach with similar structures, but solve different problems:
- **Dijkstra's Algorithm** finds the shortest path from a single source to all other vertices (minimizing path sum).
- **Prim's Algorithm** finds a Minimum Spanning Tree (MST) that connects all vertices together with the minimum total edge weight.

### Q20: What is the difference between Dijkstra and the Floyd-Warshall Algorithm?
**Answer**: 
- **Dijkstra** is a Single-Source Shortest Path algorithm with time complexity $O(V^2)$ (or $O((V+E)\log V)$ with a heap).
- **Floyd-Warshall** computes All-Pairs Shortest Paths (between every pair of vertices) using dynamic programming in $O(V^3)$ time. Running Dijkstra from all $V$ nodes achieves the same result in $V \times O(V^2) = O(V^3)$ time.

### Q21: What happens if you try to add more than 50 locations?
**Answer**: `addLocation()` checks `if (numLocations >= MAX)` and cleanly rejects the addition with an error message, preventing array index out-of-bounds errors.

### Q22: Why is the graph matrix initialized before loading sample data?
**Answer**: `initGraph()` sets all off-diagonal cells to `INF` and diagonal cells to `0`. Without this step, global memory would initialize cells to `0`, making the program interpret missing paths as zero-distance connections.

### Q23: Why do we reverse the path when printing in `printPath()`?
**Answer**: Because the `parent[]` array stores predecessor links (pointing backward from destination toward source). Tracing these links yields the route in reverse order ($dest \to \dots \to src$). We collect these nodes and iterate backward to display the natural forward journey ($src \to \dots \to dest$).

### Q24: Can your implementation handle directed pathways (e.g., a one-way security turnstile)?
**Answer**: Yes. To make an edge one-way from $u$ to $v$, we would assign `graph[u][v] = distance` while leaving `graph[v][u] = INF`. The underlying Dijkstra implementation handles directed edges without modification.

### Q25: Why is `cin.ignore()` used throughout `main()`?
**Answer**: When reading numbers with `cin >> choice`, the numeric value is consumed but the trailing newline character (`\n`) remains in the input buffer. Calling `cin.ignore(1000, '\n')` clears this leftover character, preventing subsequent `getline()` calls from immediately reading an empty string.

---

## 17. PRESENTATION SCRIPT (3–5 MINUTE TALK)

**[Slide 1: Title & Introduction — 0:00 to 0:45]**
> "Good morning, respected faculty and examiners. Today, I am presenting my Data Structures and Algorithms project: **'Campus Navigation and Shortest Path Finder using Dijkstra's Algorithm'**.
> 
> When navigating large university campuses or hospital complexes, finding the shortest walking route between buildings can be difficult for new students and visitors. My project models a campus as a weighted, undirected graph and computes optimal walking routes between any two facilities using Dijkstra's algorithm.
> 
> To keep the implementation clear, transparent, and easy to evaluate, the application is written in clean C++ using pure fundamental arrays, an adjacency matrix, and basic loops—avoiding complex STL containers like priority queues or dynamic vectors."

**[Slide 2: Core Data Structures & Architecture — 0:45 to 1:45]**
> "Let's examine how the campus is represented in memory.
> 
> We assign each building a unique index from 0 to 49, with names stored in a simple array: `string locations[50]`.
> 
> Campus pathways are stored in a 50 by 50 2D array: `int graph[50][50]`, functioning as an **Adjacency Matrix**. If building $A$ and building $B$ are connected by a 150-meter sidewalk, both `graph[A][B]` and `graph[B][A]` store 150. If no direct path exists, the cell holds a defined infinity sentinel of $10^9$. The diagonal is zero, representing zero distance from a building to itself.
> 
> During pathfinding, Dijkstra's algorithm uses three arrays:
> - `dist[50]`, tracking the shortest distance discovered so far.
> - `visited[50]`, tracking nodes whose shortest paths are finalized.
> - `parent[50]`, recording the predecessor of each node so we can reconstruct the full turn-by-turn route."

**[Slide 3: Dijkstra's Algorithm Mechanics — 1:45 to 2:45]**
> "Here is how our algorithm executes:
> 
> We initialize the source node's distance to 0 and all other nodes to infinity.
> 
> In each iteration, a simple `for` loop scans the unvisited nodes to find the one with the minimum tentative distance. Once selected, we mark it as `visited`—this is the greedy choice that guarantees optimality because all path weights are positive.
> 
> Next, an inner loop examines all adjacent neighbors and performs **edge relaxation**: if traveling through our current node provides a shorter route to a neighbor than previously recorded, we update its distance in `dist[]` and update its predecessor in `parent[]`.
> 
> Once the destination is reached, we trace the `parent[]` array backward to produce the turn-by-turn route and total distance. This array-based approach has a time complexity of $O(V^2)$ and a space complexity of $O(V^2)$, which executes in microseconds for our 50-node campus."

**[Slide 4: Live Demonstration & Features — 2:45 to 3:45]**
> "Our application includes 10 pre-loaded campus buildings and 13 walkways.
> 
> In the live demo, choosing Menu Option 1 displays all campus facilities. Option 2 prints the formatted adjacency matrix. 
> 
> When we run Option 3 from `MainGate` to `Auditorium`, the system evaluates multiple alternate routes—such as going through the Cafeteria or the Engineering departments—and selects the optimal route via `AdminBlock`, `CSE_Dept`, and `Library`, totaling 720 meters.
> 
> Users can also add new buildings with Option 4, connect them to existing paths with Option 5, and immediately route through them. Input validation handles non-existent buildings, identical start and end points, disconnected locations, and invalid menu choices gracefully."

**[Slide 5: Conclusion & Future Scope — 3:45 to 4:15]**
> "In conclusion, this project demonstrates how foundational graph algorithms solve practical navigation problems without requiring complex libraries.
> 
> In the future, this engine could be expanded with persistent file storage, a 2D graphical map layout, or GPS integration for mobile devices.
> 
> Thank you for your time. I am now ready for your questions."

---

## 18. IMPLEMENTATION REVIEW (CODE AUDIT & RECOMMENDATIONS)

An audit of the current source code in `main.cpp` identified the following edge cases, design trade-offs, and recommended enhancements. *(Per project guidelines, these observations are documented here for discussion without modifying the current source code).*

---

### Audit Finding 1: Potential Array Overflow in Path Reconstruction
- **Location**: `printPath()`, lines 286–294:
  ```cpp
  int path[MAX];
  int len = 0;
  int cur = dest;

  while (cur != -1) {
      path[len] = cur;
      len++;
      cur = parent[cur];
  }
  ```
- **Analysis**: In a valid graph without negative cycles, a simple path contains at most $V$ vertices ($len \le \text{numLocations} \le 50$), so `path[MAX]` will not overflow. However, if uninitialized memory or an unexpected cycle ever corrupted `parent[]`, this loop could spin indefinitely and overflow `path[50]`.
- **Recommended Fix**: Add a defensive bound check to the loop condition:
  ```cpp
  while (cur != -1 && len < MAX) {
      path[len++] = cur;
      cur = parent[cur];
  }
  ```

---

### Audit Finding 2: Implicit Location Insertion in `addPath()` Without Capacity Checking
- **Location**: `addPath()`, lines 117–118:
  ```cpp
  if (findIndex(from) == -1) addLocation(from);
  if (findIndex(to)   == -1) addLocation(to);
  ```
- **Analysis**: If `numLocations` is already at capacity (49 or 50) and a user attempts to add a path between two new locations, `addLocation()` prints an error message and leaves the node unadded. `addPath()` then checks `if (u == -1 || v == -1)` and cleanly aborts:
  ```cpp
  if (u == -1 || v == -1) {
      cout << "  [!] Could not add path.\n";
      return;
  }
  ```
  The program handles this safely, but checking capacity upfront before attempting the insertion would be slightly cleaner.

---

### Audit Finding 3: Case-Sensitive String Matching
- **Location**: `findIndex()`, lines 78–84:
  ```cpp
  if (locations[i] == name)
      return i;
  ```
- **Analysis**: Lookups rely on exact string equality. Entering `"maingate"` will not match `"MainGate"`.
- **Recommended Fix**: Normalize strings to lowercase before comparison, or provide a hint listing available locations when an entry is not found.

---

### Audit Finding 4: Single-Token Column Alignment in Matrix Display
- **Location**: `showMatrix()`, lines 174 and 181:
  ```cpp
  cout << setw(8) << locations[j].substr(0, 6);
  // ...
  cout << setw(15) << left << locations[i].substr(0, 15) << right << " |  ";
  ```
- **Analysis**: `substr(0, 6)` truncates names for clean column alignment when displaying 10 locations. If `numLocations` grows to 30 or more, the matrix will wrap across standard 80-column terminal windows.
- **Recommended Fix**: This is an inherent limitation of standard terminal widths. For larger graphs, displaying adjacency lists or paginating matrix output is recommended.

---

### Audit Finding 5: Interactive Loop Input Clearing
- **Location**: `main()`, lines 431–437:
  ```cpp
  if (cin.fail()) {
      cin.clear();
      cin.ignore(1000, '\n');
      cout << "  [!] Please enter a number from 1 to 6.\n";
      continue;
  }
  cin.ignore(1000, '\n');
  ```
- **Analysis**: The input-clearing logic works reliably for standard console usage. Using `numeric_limits<streamsize>::max()` (from `<limits>`) instead of the literal `1000` is the standard idiomatic C++ approach for clearing arbitrarily long lines, but `1000` is sufficient for typical keyboard input.

---
*Report compiled and verified against `main.cpp`.*

