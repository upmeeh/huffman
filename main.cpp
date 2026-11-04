// ============================================================
//  Chandigarh University Campus Navigation System
//  DSA Project | Shortest Path Finder using Dijkstra's Algorithm
//
//  Platform: C++17 (Terminal Application)
//  Data Structures:
//    - string locations[80]   : Names of campus landmarks & junctions
//    - string categories[80]  : Landmark category (Academic, Gate, etc.)
//    - bool   isJunction[80]  : Flag distinguishing road junctions
//    - int    graph[80][80]   : Adjacency matrix with walking distances (m)
//    - int    dist[80]        : Tentative shortest distances from source
//    - bool   visited[80]     : Finalized status tracker for Dijkstra
//    - int    parent[80]      : Predecessor array for path reconstruction
//
//  Algorithm:
//    - Pure array-based Dijkstra's Algorithm (O(V^2))
//    - Nested for-loops with linear minimum-distance vertex selection
//    - Strictly NO priority_queue, heaps, vectors, or external libraries
// ============================================================

#include <iostream>
#include <string>
#include <climits>
#include <iomanip>
using namespace std;

// ============================================================
//  CONSTANTS
// ============================================================
const int MAX = 80;       // Capacity expanded to accommodate landmarks + junctions
const int INF = 1e9;      // Sentinel representing no direct connection (1,000,000,000)

// ============================================================
//  GLOBAL DATA STRUCTURES
// ============================================================
string locations[MAX];    // Identifier of each node
string categories[MAX];   // Category label for organization
bool   isJunction[MAX];   // True if internal road junction, false if user landmark
int    graph[MAX][MAX];   // Symmetric adjacency matrix for walking distances (metres)
int    numLocations = 0;  // Total count of registered nodes

// ============================================================
//  HELPER: trim
//  Removes leading and trailing whitespace, tabs, and '\r'
//  to guarantee reliable console input processing on Windows.
// ============================================================
string trim(string s) {
    int start = 0;
    int end   = (int)s.size() - 1;
    while (start <= end && (s[start] == ' ' || s[start] == '\t' || s[start] == '\r'))
        start++;
    while (end >= start && (s[end] == ' ' || s[end] == '\t' || s[end] == '\r'))
        end--;
    return s.substr(start, end - start + 1);
}

// ============================================================
//  FUNCTION: initGraph
//  Clears the adjacency matrix:
//    - graph[i][i] = 0   (zero walking distance to self)
//    - graph[i][j] = INF (disconnected by default)
// ============================================================
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

// ============================================================
//  FUNCTION: findIndex
//  Performs an exact linear search across registered locations.
//  Returns integer index if found, -1 otherwise.
// ============================================================
int findIndex(string name) {
    for (int i = 0; i < numLocations; i++) {
        if (locations[i] == name)
            return i;
    }
    return -1;
}

// ============================================================
//  FUNCTION: addNode
//  Internal method to register a landmark or road junction.
// ============================================================
int addNode(string name, string category, bool junctionFlag) {
    if (numLocations >= MAX) {
        cout << "  [!] Node limit reached (capacity: " << MAX << ").\n";
        return -1;
    }
    int existing = findIndex(name);
    if (existing != -1) {
        return existing;
    }
    int idx = numLocations;
    locations[idx]  = name;
    categories[idx] = category;
    isJunction[idx] = junctionFlag;
    numLocations++;
    return idx;
}

// ============================================================
//  FUNCTION: addLocation
//  User-facing function to register a new campus landmark.
// ============================================================
void addLocation(string name, string category = "Custom Landmark") {
    if (name.empty()) {
        cout << "  [!] Location name cannot be empty.\n";
        return;
    }
    if (findIndex(name) != -1) {
        cout << "  [!] Location '" << name << "' already exists.\n";
        return;
    }
    int idx = addNode(name, category, false);
    if (idx != -1) {
        cout << "  [+] Added Landmark: " << name << " [" << category << "]\n";
    }
}

// ============================================================
//  FUNCTION: addPath
//  Adds an undirected walking path with a positive distance.
//  Updates both graph[u][v] and graph[v][u] symmetrically.
// ============================================================
void addPath(string from, string to, int distance) {
    if (distance <= 0) {
        cout << "  [!] Path distance must be a positive integer in metres.\n";
        return;
    }

    int u = findIndex(from);
    int v = findIndex(to);

    if (u == -1) {
        u = addNode(from, "Custom Landmark", false);
    }
    if (v == -1) {
        v = addNode(to, "Custom Landmark", false);
    }

    if (u == -1 || v == -1) {
        cout << "  [!] Failed to add path: node limit reached.\n";
        return;
    }

    graph[u][v] = distance;
    graph[v][u] = distance;

    cout << "  [+] Path: " << from << " <--> " << to << " (" << distance << " m)\n";
}

// ============================================================
//  FUNCTION: showLocations
//  Displays all verified Chandigarh University landmarks
//  organized by real functional categories.
// ============================================================
void showLocations() {
    cout << "\n=======================================================\n";
    cout << "        CHANDIGARH UNIVERSITY CAMPUS LANDMARKS        \n";
    cout << "=======================================================\n";

    string currentCats[] = {
        "Campus Gate",
        "Academic Block (A)",
        "Academic Block (B)",
        "Academic Block (C)",
        "Academic Block (D)",
        "Student Residence",
        "Campus Amenities & Sports",
        "Custom Landmark"
    };
    int catCount = 8;

    for (int c = 0; c < catCount; c++) {
        bool headerPrinted = false;
        for (int i = 0; i < numLocations; i++) {
            if (!isJunction[i] && categories[i] == currentCats[c]) {
                if (!headerPrinted) {
                    cout << "\n  -- " << currentCats[c] << "s --\n";
                    headerPrinted = true;
                }
                cout << "    [" << right << setw(2) << i << "]  " << left << setw(22) << locations[i] << "\n";
            }
        }
    }

    // Optional list of internal road junctions
    cout << "\n  -- Internal Walking Road Junctions --\n";
    int jCount = 0;
    for (int i = 0; i < numLocations; i++) {
        if (isJunction[i]) {
            cout << "    (" << right << setw(2) << i << ")  " << left << setw(22) << locations[i];
            jCount++;
            if (jCount % 2 == 0) cout << "\n";
        }
    }
    if (jCount % 2 != 0) cout << "\n";

    cout << "-------------------------------------------------------\n";
    cout << "  Total Locations Registered: " << numLocations << "\n";
    cout << "=======================================================\n\n";
}

// ============================================================
//  FUNCTION: showMatrix
//  Displays the adjacency matrix showing walking distances.
// ============================================================
void showMatrix() {
    if (numLocations == 0) {
        cout << "  No campus graph data to display.\n";
        return;
    }

    cout << "\n=== ADJACENCY MATRIX (Walking distances in metres) ===\n\n";

    cout << setw(18) << " ";
    for (int j = 0; j < numLocations; j++) {
        cout << setw(7) << locations[j].substr(0, 5);
    }
    cout << "\n" << string(18 + numLocations * 7, '-') << "\n";

    for (int i = 0; i < numLocations; i++) {
        cout << setw(15) << left << locations[i].substr(0, 15) << right << " | ";
        for (int j = 0; j < numLocations; j++) {
            if (graph[i][j] == INF)
                cout << setw(7) << "---";
            else
                cout << setw(7) << graph[i][j];
        }
        cout << "\n";
    }
    cout << string(18 + numLocations * 7, '=') << "\n\n";
}

// ============================================================
//  FUNCTION: dijkstra
//  Fundamental Array-Based Dijkstra's Shortest Path Algorithm
//
//  Complexity:
//    - Time:  O(V^2) via nested loops and linear search
//    - Space: O(V) auxiliary memory
//
//  Steps:
//    1. Initialize dist[] to INF, visited[] to false, parent[] to -1.
//    2. Set dist[src] = 0.
//    3. Repeat V times:
//         a. Find unvisited node u with minimum tentative dist[u].
//         b. Mark u as visited (finalized).
//         c. Relax all unvisited adjacent neighbors v.
// ============================================================
void dijkstra(int src, int dist[], int parent[]) {
    bool visited[MAX];

    // Step 1: Initialization
    for (int i = 0; i < numLocations; i++) {
        dist[i]    = INF;
        visited[i] = false;
        parent[i]  = -1;
    }
    dist[src] = 0;

    // Step 2: Main relaxation loop
    for (int count = 0; count < numLocations; count++) {

        // 2a. Find unvisited vertex with minimum tentative distance
        int u    = -1;
        int minD = INF;

        for (int i = 0; i < numLocations; i++) {
            if (!visited[i] && dist[i] < minD) {
                minD = dist[i];
                u    = i;
            }
        }

        // If remaining unvisited vertices are unreachable, break early
        if (u == -1) break;

        // 2b. Mark vertex as visited
        visited[u] = true;

        // 2c. Relax adjacent edges
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

// ============================================================
//  FUNCTION: printPath
//  Reconstructs and formats the shortest walking route
//  from predecessor links stored in parent[].
// ============================================================
void printPath(int src, int dest, int dist[], int parent[]) {
    cout << "\n+--------------------------------------------------------------+\n";
    cout << "                   SHORTEST WALKING ROUTE                       \n";
    cout << "+--------------------------------------------------------------+\n";
    cout << "  Starting Landmark : " << locations[src]  << " (" << categories[src] << ")\n";
    cout << "  Destination       : " << locations[dest] << " (" << categories[dest] << ")\n";
    cout << "+--------------------------------------------------------------+\n";

    if (dist[dest] == INF) {
        cout << "\n  [!] UNREACHABLE: No connected walking path exists between\n";
        cout << "      \"" << locations[src] << "\" and \"" << locations[dest] << "\".\n\n";
        cout << "+--------------------------------------------------------------+\n\n";
        return;
    }

    // Trace path backward from dest to src
    int path[MAX];
    int len = 0;
    int cur = dest;

    while (cur != -1 && len < MAX) {
        path[len++] = cur;
        cur = parent[cur];
    }

    // Display forward route sequence
    cout << "\n  Complete Route:\n  ";
    for (int i = len - 1; i >= 0; i--) {
        if (isJunction[path[i]]) {
            cout << "[" << locations[path[i]] << "]";
        } else {
            cout << locations[path[i]];
        }
        if (i > 0) cout << " -> ";
    }
    cout << "\n\n";

    // Display segment-by-segment walking itinerary
    cout << "  Step-by-Step Walking Directions:\n";
    int step = 1;
    for (int i = len - 1; i > 0; i--) {
        int u = path[i];
        int v = path[i - 1];
        cout << "    Step " << setw(2) << step++ << ": "
             << left << setw(22) << locations[u]
             << " -->  "
             << left << setw(22) << locations[v]
             << right << "(" << setw(4) << graph[u][v] << " m)";
        if (isJunction[v]) {
            cout << "  [Road Junction]";
        }
        cout << "\n";
    }

    cout << "\n  Total Walking Distance: " << dist[dest] << " metres (approx. "
         << fixed << setprecision(1) << (dist[dest] / 80.0) << " mins walking)\n";
    cout << "+--------------------------------------------------------------+\n\n";
}

// ============================================================
//  FUNCTION: findShortestRoute
//  Validates user inputs, executes Dijkstra, and prints results.
// ============================================================
void findShortestRoute(string from, string to) {
    int src  = findIndex(from);
    int dest = findIndex(to);

    if (src == -1) {
        cout << "  [!] Unknown location: \"" << from << "\". Check menu option 1 for valid names.\n";
        return;
    }
    if (dest == -1) {
        cout << "  [!] Unknown location: \"" << to << "\". Check menu option 1 for valid names.\n";
        return;
    }
    if (src == dest) {
        cout << "  [!] Starting point and destination are identical: \"" << from
             << "\" (0 metres, 0 mins).\n";
        return;
    }

    int dist[MAX];
    int parent[MAX];

    dijkstra(src, dist, parent);
    printPath(src, dest, dist, parent);
}

// ============================================================
//  FUNCTION: loadChandigarhUniversityData
//  Preloads all 28 verified Chandigarh University landmarks
//  and models the walking road network with internal junctions.
// ============================================================
void loadChandigarhUniversityData() {
    cout << "  Loading Chandigarh University Campus Topology...\n";

    // --- 1. GATES (3) ---
    addNode("Gate 1",                  "Campus Gate", false);
    addNode("Gate 2",                  "Campus Gate", false);
    addNode("Gate 4",                  "Campus Gate", false);

    // --- 2. ACADEMIC BLOCKS: A-SERIES (3) ---
    addNode("A1",                      "Academic Block (A)", false);
    addNode("A2",                      "Academic Block (A)", false);
    addNode("A3",                      "Academic Block (A)", false);

    // --- 3. ACADEMIC BLOCKS: B-SERIES (4) ---
    addNode("B1",                      "Academic Block (B)", false);
    addNode("B2",                      "Academic Block (B)", false);
    addNode("B3",                      "Academic Block (B)", false);
    addNode("B4",                      "Academic Block (B)", false);

    // --- 4. ACADEMIC BLOCKS: C-SERIES (3) ---
    addNode("C1",                      "Academic Block (C)", false);
    addNode("C2",                      "Academic Block (C)", false);
    addNode("C3",                      "Academic Block (C)", false);

    // --- 5. ACADEMIC BLOCKS: D-SERIES (7) ---
    addNode("D1",                      "Academic Block (D)", false);
    addNode("D2",                      "Academic Block (D)", false);
    addNode("D3",                      "Academic Block (D)", false);
    addNode("D4",                      "Academic Block (D)", false);
    addNode("D5",                      "Academic Block (D)", false);
    addNode("D6",                      "Academic Block (D)", false);
    addNode("D7",                      "Academic Block (D)", false);

    // --- 6. HOSTEL RESIDENCES (3) ---
    addNode("Sukhna Hostel",           "Student Residence", false);
    addNode("LC Hostel",               "Student Residence", false);
    addNode("Boys Hostel",             "Student Residence", false);

    // --- 7. CAMPUS AMENITIES & SPORTS (5) ---
    addNode("Fountain Park",           "Campus Amenities & Sports", false);
    addNode("Basketball Court",        "Campus Amenities & Sports", false);
    addNode("CU Bus Parking",          "Campus Amenities & Sports", false);
    addNode("University Ground",       "Campus Amenities & Sports", false);
    addNode("Shopping Complex",        "Campus Amenities & Sports", false);

    // --- 8. INTERNAL WALKING ROAD JUNCTIONS ---
    addNode("J_Gate1",                 "Road Junction", true);
    addNode("J_MainAvenue",            "Road Junction", true);
    addNode("J_Gate2",                 "Road Junction", true);
    addNode("J_BlockA",                "Road Junction", true);
    addNode("J_FountainPark",          "Road Junction", true);
    addNode("J_BlockB",                "Road Junction", true);
    addNode("J_ShoppingComplex",       "Road Junction", true);
    addNode("J_BlockC",                "Road Junction", true);
    addNode("J_AcademicSpine",         "Road Junction", true);
    addNode("J_BlockD_North",          "Road Junction", true);
    addNode("J_BlockD_South",          "Road Junction", true);
    addNode("J_SportsZone",            "Road Junction", true);
    addNode("J_Sukhna",                "Road Junction", true);
    addNode("J_HostelHub",             "Road Junction", true);
    addNode("J_Gate4",                 "Road Junction", true);

    // ========================================================
    //  WALKING ROAD CONNECTIONS & DISTANCES (in metres)
    //  Modeled strictly along walking roads and pedestrian corridors
    // ========================================================

    // Gate 1 Entrance Corridor & Parking
    addPath("Gate 1",            "J_Gate1",            30);
    addPath("CU Bus Parking",    "J_Gate1",            80);
    addPath("J_Gate1",           "J_MainAvenue",      120);

    // Gate 2 Lateral Transit Access
    addPath("Gate 2",            "J_Gate2",            40);
    addPath("J_Gate2",           "J_MainAvenue",      160);
    addPath("J_Gate2",           "J_ShoppingComplex", 190);

    // Main Avenue to Block A and Central Zone
    addPath("J_MainAvenue",      "J_BlockA",          100);
    addPath("A1",                "J_BlockA",           40);
    addPath("A2",                "J_BlockA",           50);
    addPath("A3",                "J_BlockA",           60);
    addPath("A1",                "A2",                 35);
    addPath("A2",                "A3",                 35);

    // Block A to Fountain Park & Block B Road
    addPath("J_BlockA",          "J_FountainPark",     90);
    addPath("Fountain Park",     "J_FountainPark",     25);
    addPath("J_FountainPark",    "J_BlockB",           80);

    // Block B Cluster Access
    addPath("B1",                "J_BlockB",           30);
    addPath("B2",                "J_BlockB",           40);
    addPath("B3",                "J_BlockB",           50);
    addPath("B4",                "J_BlockB",           60);
    addPath("B1",                "B2",                 30);
    addPath("B2",                "B3",                 30);
    addPath("B3",                "B4",                 30);

    // Shopping Complex Linkage
    addPath("J_BlockB",          "J_ShoppingComplex",  70);
    addPath("Shopping Complex",  "J_ShoppingComplex",  20);

    // Central Corridor to Block C
    addPath("J_ShoppingComplex", "J_BlockC",          110);
    addPath("J_BlockB",          "J_BlockC",          120);
    addPath("C1",                "J_BlockC",           35);
    addPath("C2",                "J_BlockC",           45);
    addPath("C3",                "J_BlockC",           55);
    addPath("C1",                "C2",                 30);
    addPath("C2",                "C3",                 30);

    // Academic Promenade to Block D Cluster
    addPath("J_BlockC",          "J_AcademicSpine",   100);
    addPath("J_AcademicSpine",   "J_BlockD_North",     80);
    addPath("D1",                "J_BlockD_North",     30);
    addPath("D2",                "J_BlockD_North",     40);
    addPath("D3",                "J_BlockD_North",     50);
    addPath("D1",                "D2",                 30);
    addPath("D2",                "D3",                 30);

    addPath("J_BlockD_North",    "J_BlockD_South",     90);
    addPath("D4",                "J_BlockD_South",     30);
    addPath("D5",                "J_BlockD_South",     40);
    addPath("D6",                "J_BlockD_South",     50);
    addPath("D7",                "J_BlockD_South",     60);
    addPath("D4",                "D5",                 30);
    addPath("D5",                "D6",                 30);
    addPath("D6",                "D7",                 30);

    // Sports Zone & Grounds
    addPath("J_AcademicSpine",   "J_SportsZone",      110);
    addPath("Basketball Court",  "J_SportsZone",       40);
    addPath("University Ground", "J_SportsZone",       70);
    addPath("J_BlockD_South",    "J_SportsZone",      130);

    // Residential Sectors: Sukhna Hostel Pathway
    addPath("J_FountainPark",    "J_Sukhna",          220);
    addPath("J_BlockB",          "J_Sukhna",          210);
    addPath("Sukhna Hostel",     "J_Sukhna",           50);

    // Residential Sectors: Southern Hostel Hub (LC & Boys Hostel)
    addPath("J_SportsZone",      "J_HostelHub",       180);
    addPath("J_BlockD_South",    "J_HostelHub",       190);
    addPath("LC Hostel",         "J_HostelHub",        60);
    addPath("Boys Hostel",       "J_HostelHub",        70);
    addPath("LC Hostel",         "Boys Hostel",        50);

    // Gate 4 Rear Transit Access
    addPath("J_HostelHub",       "J_Gate4",            90);
    addPath("Gate 4",            "J_Gate4",            30);

    cout << "  [OK] Chandigarh University Campus Graph Initialized successfully!\n";
    cout << "       - 28 Verified Landmarks across 4 Categories\n";
    cout << "       - 15 Walking Road Junctions\n";
    cout << "       - Realistic Connected Pedestrian Graph\n\n";
}

// ============================================================
//  FUNCTION: showMenu
// ============================================================
void showMenu() {
    cout << "\n=======================================================\n";
    cout << "      CHANDIGARH UNIVERSITY CAMPUS NAVIGATOR          \n";
    cout << "=======================================================\n";
    cout << "  1. Show all campus locations (categorized)\n";
    cout << "  2. Show campus walking matrix (distances in m)\n";
    cout << "  3. Find shortest walking route (Dijkstra's Algorithm)\n";
    cout << "  4. Add a new campus location\n";
    cout << "  5. Add a new walking path between locations\n";
    cout << "  6. Exit\n";
    cout << "=======================================================\n";
    cout << "  Enter your choice (1-6): ";
}

// ============================================================
//  MAIN FUNCTION
// ============================================================
int main() {
    cout << "=======================================================\n";
    cout << "     CHANDIGARH UNIVERSITY CAMPUS NAVIGATION SYSTEM    \n";
    cout << "   Data Structures & Algorithms - Dijkstra Project     \n";
    cout << "=======================================================\n\n";

    initGraph();
    loadChandigarhUniversityData();

    int choice;
    while (true) {
        showMenu();
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "  [!] Invalid input. Please enter a number between 1 and 6.\n";
            continue;
        }
        cin.ignore(1000, '\n');

        if (choice == 1) {
            showLocations();

        } else if (choice == 2) {
            showMatrix();

        } else if (choice == 3) {
            cout << "\n  --- FIND SHORTEST WALKING ROUTE ---\n";
            cout << "  Enter STARTING location name (e.g., Gate 1, A1, Sukhna Hostel): ";
            string src;
            getline(cin, src);
            src = trim(src);

            cout << "  Enter DESTINATION location name (e.g., D3, Boys Hostel, Gate 4): ";
            string dst;
            getline(cin, dst);
            dst = trim(dst);

            findShortestRoute(src, dst);

        } else if (choice == 4) {
            cout << "\n  --- ADD NEW CAMPUS LOCATION ---\n";
            cout << "  Enter location name: ";
            string name;
            getline(cin, name);
            name = trim(name);

            cout << "  Enter category (e.g., Academic Block, Lab, Canteen): ";
            string cat;
            getline(cin, cat);
            cat = trim(cat);
            if (cat.empty()) cat = "Custom Landmark";

            addLocation(name, cat);

        } else if (choice == 5) {
            cout << "\n  --- ADD NEW WALKING PATH ---\n";
            cout << "  Enter FIRST location name  : ";
            string u;
            getline(cin, u);
            u = trim(u);

            cout << "  Enter SECOND location name : ";
            string v;
            getline(cin, v);
            v = trim(v);

            cout << "  Enter walking distance in metres: ";
            int dist;
            cin >> dist;

            if (cin.fail()) {
                cin.clear();
                cin.ignore(1000, '\n');
                cout << "  [!] Invalid distance. Please enter a positive integer.\n";
            } else {
                cin.ignore(1000, '\n');
                addPath(u, v, dist);
            }

        } else if (choice == 6) {
            cout << "\n  Thank you for using Chandigarh University Campus Navigator!\n";
            cout << "  Safe travels across campus. Goodbye!\n\n";
            break;

        } else {
            cout << "  [!] Invalid choice. Please choose a number between 1 and 6.\n";
        }
    }

    return 0;
}
