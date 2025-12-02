#include "CampusCompass.h"
#include <iostream>
#include <sstream>
#include <regex>
#include <fstream>
#include <climits>
#include <queue>

using namespace std;

CampusCompass::CampusCompass() {
    // initialize any members if needed
}

bool CampusCompass::ParseCSV(const string &edges_filepath, const string &classes_filepath) {
    bool ok1 = LoadEdges(edges_filepath);
    bool ok2 = LoadClasses(classes_filepath);
    return ok1 && ok2;
}

bool CampusCompass::LoadEdges(const string &filepath) {
    ifstream file(filepath);
    if (!file.is_open()) return false;

    string line;
    getline(file, line);

    while (getline(file, line)) {
        stringstream ss(line);
        string a, b, name1, name2, t;

        getline(ss, a, ',');
        getline(ss, b, ',');
        getline(ss, name1, ',');
        getline(ss, name2, ',');
        getline(ss, t, ',');

        int u = stoi(a);
        int v = stoi(b);
        int w = stoi(t);

        graph[u].push_back({v, w, true});
        graph[v].push_back({u, w, true});

        idToName[u] = name1;
        idToName[v] = name2;
    }
    return true;
}

bool CampusCompass::LoadClasses(const string &filepath) {
    ifstream file(filepath);
    if (!file.is_open()) return false;

    string line;
    getline(file, line);

    while (getline(file, line)) {
        stringstream ss(line);
        string code, loc, start, end;

        getline(ss, code, ',');
        getline(ss, loc, ',');
        getline(ss, start, ',');
        getline(ss, end, ',');

        ClassInfo c{code, stoi(loc), start, end};
        classes[code] = c;
    }
    return true;
}

bool CampusCompass::ParseCommand(const string &command) {
    vector<string> commands = Commands(command);
    if (commands.empty()) {
        cout << "unsuccessful" << endl;
        return false;
    }

    string cmd = commands[0];

    if (cmd == "insert") return HandleInsert(commands);
    if (cmd == "remove") return HandleRemove(commands);
    if (cmd == "dropClass") return HandleDropClass(commands);
    if (cmd == "replaceClass") return HandleReplaceClass(commands);
    if (cmd == "removeClass") return HandleRemoveClassGlobal(commands);
    if (cmd == "toggleEdgesClosure") return HandleToggleEdges(commands);
    if (cmd == "checkEdgeStatus") return HandleCheckEdgeStatus(commands);
    if (cmd == "isConnected") return HandleIsConnected(commands);
    if (cmd == "printShortestEdges") return HandlePrintShortestEdges(commands);
    if (cmd == "printStudentZone") return HandlePrintStudentZone(commands);

    cout << "unsuccessful" << endl;
    return false;
}


vector<string> CampusCompass::Commands(const string &input) {
    vector<string> commands;
    regex re("\"([^\"]*)\"|(\\S+)");
    auto words_begin = sregex_iterator(input.begin(), input.end(), re);
    auto words_end = sregex_iterator();

    for (auto i = words_begin; i != words_end; ++i) {
        smatch match = *i;
        string command;

        if (match[1].matched)
            command = match[1].str();
        else
            command = match[2].str();

        commands.push_back(command);
    }

    return commands;
}

bool CampusCompass::IsValidUFID(const string &id) {
    return regex_match(id, regex("^[0-9]{8}$"));
}

bool CampusCompass::IsValidName(const string &name) {
    return regex_match(name, regex("^[A-Za-z ]+$"));
}

bool CampusCompass::IsValidClassCode(const string &code) {
    return regex_match(code, regex("^[A-Z]{3}[0-9]{4}$"));
}

bool CampusCompass::HandleInsert(const vector<string>& commands) {
    // Must have at least 5 tokens: insert name ufid residence N
    if (commands.size() < 5) {
        cout << "unsuccessful" << endl;
        return false;
    }

    string name = commands[1];
    string ufid = commands[2];
    string residenceStr = commands[3];
    string nStr = commands[4];

    // Validate name and UFID
    if (!IsValidName(name) || !IsValidUFID(ufid)) {
        cout << "unsuccessful" << endl;
        return false;
    }

    // Ensure UFID is unique
    if (students.count(ufid)) {
        cout << "unsuccessful" << endl;
        return false;
    }

    // Convert residence and N
    int residence, n;
    try {
        residence = stoi(residenceStr);
        n = stoi(nStr);
    } catch (...) {
        cout << "unsuccessful" << endl;
        return false;
    }

    // Validate number of classes
    if (n < 1 || n > 6 || commands.size() != 5 + n) {
        cout << "unsuccessful" << endl;
        return false;
    }

    // Validate class codes
    vector<string> classList;
    for (int i = 0; i < n; i++) {
        string code = commands[5 + i];
        if (!IsValidClassCode(code)) {
            cout << "unsuccessful" << endl;
            return false;
        }
        classList.push_back(code);
    }

    // Create student and store
    Student s{name, ufid, residence, classList};
    students[ufid] = s;

    cout << "successful" << endl;
    return true;
}

bool CampusCompass::HandleRemove(const vector<string>& commands) {
    if (commands.size() < 2 || commands.size() > 2) {
        cout << "unsuccessful" << endl;
        return false;
    }

    string ufid = commands[1];

    if (!IsValidUFID(ufid) || students.find(ufid) == students.end()) {
        cout << "unsuccessful" << endl;
        return false;
    }
    
    students.erase(ufid);
    cout << "successful" << endl;
    return true;
}

bool CampusCompass::HandleDropClass(const vector<string>& commands) {
    if (commands.size() < 3 || commands.size() > 3) {
        cout << "unsuccessful" << endl;
        return false;
    }

    string ufid = commands[1];
    string classCode = commands[2];

    if (!IsValidUFID(ufid) || !IsValidClassCode(classCode) || students.find(ufid) == students.end()) {
        cout << "unsuccessful" << endl;
        return false;
    }

    Student &student = students[ufid];
    auto c = find(student.classes.begin(), student.classes.end(), classCode);
    if (c == student.classes.end()) {
        cout << "unsuccessful" << endl;
        return false;
    }
    student.classes.erase(c);

    cout << "successful" << endl;
    return true;
}

bool CampusCompass::HandleReplaceClass(const vector<string>& commands) {
    if (commands.size() < 4 || commands.size() > 4) {
        cout << "unsuccessful" << endl;
        return false;
    }

    string ufid = commands[1];
    string class1 = commands[2];
    string class2 = commands[3];

    if (!IsValidUFID(ufid) || !IsValidClassCode(class1) || !IsValidClassCode(class2) || students.find(ufid) == students.end()) {
        cout << "unsuccessful" << endl;
        return false;
    }

    Student &student = students[ufid];
    auto c = find(student.classes.begin(), student.classes.end(), class1);
    if (c == student.classes.end()) {
        cout << "unsuccessful" << endl;
        return false;
    }
    student.classes.erase(c);
    student.classes.push_back(class2);

    cout << "successful" << endl;
    return true;
}

bool CampusCompass::HandleRemoveClassGlobal(const vector<string>& commands) {
    if (commands.size() < 2 || commands.size() > 2) {
        cout << "unsuccessful" << endl;
        return false;
    }

    string classCode = commands[1];
    if (!IsValidClassCode(classCode)) {
        cout << "unsuccessful" << endl;
        return false;
    }

    int count = 0;

    for (auto& pair : students) {
        Student& student = pair.second;

        if (find(student.classes.begin(), student.classes.end(), classCode) != student.classes.end()) {
            student.classes.erase(
                remove(student.classes.begin(), student.classes.end(), classCode),
                student.classes.end()
            );

            count++;
        }
    }

    cout << count << endl;
    return true;
}

bool CampusCompass::HandleToggleEdges(const vector<string>& commands) {
    if (commands.size() < 3) {
        cout << "unsuccessful" << endl;
        return false;
    }

    string Nstr = commands[1];
    int N = stoi(Nstr);

    if (commands.size() != N * 2 + 2) {
        cout << "unsuccessful" << endl;
        return false;
    }

    for (int i = 0; i < N*2; i++) {
        string idStr = commands[2 + i];
        int id = stoi(idStr);

        for (auto& edge : graph[id]) {
            if (edge.open) {
                edge.open = false;
            } else {
                edge.open = true;
            }
        }
    }

    cout << "successful" << endl;
    return true;
}

bool CampusCompass::HandleCheckEdgeStatus(const vector<string>& commands) {
    if (commands.size() < 3 || commands.size() > 3) {
        cout << "unsuccessful" << endl;
        return false;
    }
    string aStr = commands[1];
    string bStr = commands[2];
    int a = stoi(aStr);
    int b = stoi(bStr);
    for (auto& edge : graph[a]) {
        if (edge.to == b) {
            if (!edge.open) {
                cout << "closed" << endl;
                return true;
            }
            if (edge.open) {
                cout << "open" << endl;
                return true;
            }
        }
    }
    cout << "DNE" << endl;
    return false;
}

bool CampusCompass::DFS(int current, int target, unordered_set<int>& visited) {
    if (current == target) return true;
    visited.insert(current);

    for (auto& edge : graph[current]) {
        if (edge.open && visited.find(edge.to) == visited.end()) {
            if (DFS(edge.to, target, visited)) {
                return true;
            }
        }
    }
    return false;
}

bool CampusCompass::HandleIsConnected(const vector<string>& commands) {
    if (commands.size() < 3 || commands.size() > 3) {
        cout << "unsuccessful" << endl;
        return false;
    }
    string aStr = commands[1];
    string bStr = commands[2];
    int a = stoi(aStr);
    int b = stoi(bStr);

    if (graph.find(a) == graph.end() || graph.find(b) == graph.end()) {
        cout << "unsuccessful" << endl;
        return false;
    }

    unordered_set<int> visited;
    bool connected = DFS(a, b, visited);

    if (connected) {
        cout << "successful" << endl;
    } 
    else {
        cout << "unsuccessful" << endl;
    }

    return connected;
}

unordered_map<int, int> dijkstra(int start, const unordered_map<int, vector<Edge>>& graph) {
    unordered_map<int, int> dist;

    for (auto& p : graph) {
        dist[p.first] = INT_MAX;
    }

    priority_queue<pair<int,int>, vector<pair<int,int>>, greater<pair<int,int>>> pq;

    dist[start] = 0;
    pq.push({0, start});

    while (!pq.empty()) {
        pair <int, int> top = pq.top();
        pq.pop();

        int d = top.first;
        int node = top.second;

        if (d > dist[node]) continue;

        for (auto& edge : graph.at(node)) {
            if (!edge.open) continue;

            int neighbor = edge.to;
            int weight = edge.weight;

            if (dist[node] + weight < dist[neighbor]) {
                dist[neighbor] = dist[node] + weight;
                pq.push({dist[neighbor], neighbor});
            }
        }
    }

    return dist;
}

bool CampusCompass::HandlePrintShortestEdges(const vector<string>& commands) {
    if (commands.size() != 2) {
        cout << "unsuccessful" << endl;
        return false;
    }

    string ufid = commands[1];
    if (!IsValidUFID(ufid) || students.find(ufid) == students.end()) {
        cout << "unsuccessful" << endl;
        return false;
    }

    Student& student = students[ufid];
    cout << "Name: " << student.name << endl;

    int begin = student.residence;

    unordered_map<int,int> distances = dijkstra(begin, graph);
    vector<string> classesSorted = student.classes;
    sort(classesSorted.begin(), classesSorted.end());

    for (const string& classCode : classesSorted) {
        if (classes.find(classCode) == classes.end()) {
            cout << classCode << " | Total Time: -1" << endl;
            continue;
        }

        ClassInfo& classInfo = classes[classCode];
        int locationID = classInfo.locationID;
        int distance = distances[locationID];

        if (distance == INT_MAX) {
            cout << classCode << " | Total Time: -1" << endl;
        }
        else {
            cout << classCode << " | Total Time: " << distance << endl;
        }
    }

    return true;
}

int CampusCompass::MSTCost(const unordered_set<int>& nodes) {
    if (nodes.empty()) return 0;

    int start = *nodes.begin();

    unordered_set<int> visited;
    visited.insert(start);

    priority_queue<tuple<int,int,int>, vector<tuple<int,int,int>>, greater<tuple<int,int,int>>> pq;

    for (const Edge& e : graph[start]) {
        if (e.open && nodes.count(e.to)) {
            pq.push({e.weight, start, e.to});
        }
    }

    int cost = 0;

    while (!pq.empty() && visited.size() < nodes.size()) {
        auto edge = pq.top();
        int w = get<0>(edge);
        int u = get<1>(edge);
        int v = get<2>(edge);

        pq.pop();

        if (visited.count(v)) continue;

        visited.insert(v);
        cost += w;

        for (const Edge& e : graph[v]) {
            if (e.open && nodes.count(e.to) && !visited.count(e.to)) {
                pq.push({e.weight, v, e.to});
            }
        }
    }

    return cost;
}

bool CampusCompass::HandlePrintStudentZone(const vector<string>& commands) {
    string id = commands[1];
    Student& s = students[id];

    unordered_map<int,int> dist = dijkstra(s.residence, graph);

    unordered_set<int> nodes;
    nodes.insert(s.residence);
    
    for (const string& code : s.classes) {
        int location = classes[code].locationID;
        if (dist[location] != INT_MAX) {
            nodes.insert(location);
        }
    }

    int cost = MSTCost(nodes);

    cout << "Student Zone Cost For " << s.name << ": " << cost << endl;
    return true;
}

bool HandleVerifyScheduleID(const vector<string>& commands) {
    return true;
}
