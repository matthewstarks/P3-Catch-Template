#include "CampusCompass.h"
#include <iostream>
#include <sstream>
#include <regex>
#include <fstream>
#include <climits>
#include <queue>
#include <string>

using namespace std;

CampusCompass::CampusCompass() {
    // initialize any members if needed
}

//parse csv for edges and classes
bool CampusCompass::ParseCSV(const string &edges_filepath, const string &classes_filepath) {
    bool ok1 = LoadEdges(edges_filepath);
    bool ok2 = LoadClasses(classes_filepath);
    return ok1 && ok2;
}

//load edges from csv
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

//load classes from csv
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

//parse and execute command
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
    if (cmd == "verifySchedule") return HandleVerifyScheduleID(commands);

    cout << "unsuccessful" << endl;
    return false;
}

//split input into segments for use
vector<string> CampusCompass::Commands(const string &input) {
    vector<string> commands;
    regex word(R"(\"([^\"]*)\"|(\S+))");
    smatch match;
    string s = input;

    while (regex_search(s, match, word)) {
        if (match[1].matched) {
            commands.push_back(match[1].str());
        } else {
            commands.push_back(match[2].str());
        }
        s = match.suffix().str();
    }

    return commands;
}

//check if ufid is valid
bool CampusCompass::IsValidUFID(const string &id) {
    return regex_match(id, regex("^[0-9]{8}$"));
}

//check if name is valid
bool CampusCompass::IsValidName(const string &name) {
    return regex_match(name, regex("^[A-Za-z ]+$"));
}

//check if class code is valid
bool CampusCompass::IsValidClassCode(const string &code) {
    return regex_match(code, regex("^[A-Z]{3}[0-9]{4}$"));
}

//insert a new student
bool CampusCompass::HandleInsert(const vector<string>& commands) {
    if (commands.size() < 5) {
        cout << "unsuccessful" << endl;
        return false;
    }

    string name = commands[1];
    string ufid = commands[2];
    string residenceStr = commands[3];
    string nStr = commands[4];

    if (!IsValidName(name) || !IsValidUFID(ufid)) {
        cout << "unsuccessful" << endl;
        return false;
    }

    if (students.count(ufid)) {
        cout << "unsuccessful" << endl;
        return false;
    }

    int residence, n;
    try {
        residence = stoi(residenceStr);
        n = stoi(nStr);
    } catch (...) {
        cout << "unsuccessful" << endl;
        return false;
    }

    if (n < 1 || n > 6 || commands.size() != 5 + n) {
        cout << "unsuccessful" << endl;
        return false;
    }

    vector<string> classList;
    for (int i = 0; i < n; i++) {
        string code = commands[5 + i];
        if (!IsValidClassCode(code)) {
            cout << "unsuccessful" << endl;
            return false;
        }
        classList.push_back(code);
    }

    Student s{name, ufid, residence, classList};
    students[ufid] = s;

    cout << "successful" << endl;
    return true;
}

//remove a student
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

//drop a class for a student
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

//replace a class for a student with another class
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

//remove a class from all students
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

//toggle edges open/closed
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

        for (int i = 0; i < N; i++) {
            int u = stoi(commands[2 + i*2]);
            int v = stoi(commands[3 + i*2]);

            for (auto& edge : graph[u]) {
                if (edge.to == v) edge.open = !edge.open;
            }
            for (auto& edge : graph[v]) {
                if (edge.to == u) edge.open = !edge.open;
            }
        }
    }

    cout << "successful" << endl;
    return true;
}

//check edge is open or closed
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

//DFS for connectivity
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

//check if two locations are connected
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

//dijkstra's
pair<unordered_map<int, int>, unordered_map<int, int>> dijkstra(int start, const unordered_map<int, vector<Edge>>& graph) {
    unordered_map<int, int> dist;
    unordered_map<int,int> prev;

    for (auto& p : graph) {
        dist[p.first] = INT_MAX;
        prev[p.first] = -1;
    }

    dist[start] = 0;
    prev[start] = start;

    priority_queue<pair<int,int>, vector<pair<int,int>>, greater<pair<int,int>>> pq;

    pq.push({0, start});

    while (!pq.empty()) {
        pair <int, int> top = pq.top();
        pq.pop();

        int d = top.first;
        int node = top.second;

        if (d > dist[node]) continue;

        if (graph.find(node) == graph.end()) continue;

        for (auto& edge : graph.at(node)) {
            if (!edge.open) continue;

            int neighbor = edge.to;
            int weight = edge.weight;

            if (dist[node] + weight < dist[neighbor]) {
                dist[neighbor] = dist[node] + weight;
                prev[neighbor] = node;
                pq.push({dist[neighbor], neighbor});
            }
        }
    }

    return {dist, prev};
}

//print shortest path distance from classes for a student
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

    unordered_map<int,int> distances = dijkstra(begin, graph).first;
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

//prim's
int CampusCompass::MSTCost(const unordered_set<int>& nodes) {
    if (nodes.empty()) {
        return 0;
    }

    int start = *nodes.begin();
    unordered_set<int> visited;
    visited.insert(start);

    priority_queue<tuple<int,int,int>, vector<tuple<int,int,int>>, greater<tuple<int,int,int>>> pq;

    for (const Edge& e : graph[start]) {
        if (e.open && nodes.count(e.to)) {
            pq.push(make_tuple(e.weight, start, e.to));
        }
    }

    int cost = 0;

    while (!pq.empty() && visited.size() < nodes.size()) {
        tuple<int,int,int> edge = pq.top();
        pq.pop();

        int w = get<0>(edge);
        int u = get<1>(edge);
        int v = get<2>(edge);

        if (visited.count(v) > 0) {
            continue;
        }

        visited.insert(v);
        cost += w;

        for (const Edge& e : graph[v]) {
            if (e.open && nodes.count(e.to) && visited.count(e.to) == 0) {
                pq.push(make_tuple(e.weight, v, e.to));
            }
        }
    }

    if (visited.size() != nodes.size()) {
        return -1;
    }

    return cost;
}

//student zone cost
bool CampusCompass::HandlePrintStudentZone(const vector<string>& commands) {
    if (commands.size() != 2) {
        cout << "unsuccessful" << endl;
        return false;
    }

    string id = commands[1];

    if (students.find(id) == students.end()) {
        cout << "unsuccessful" << endl;
        return false;
    }

    Student& student = students[id];

    unordered_map<int,int> distances;
    unordered_map<int,int> previous;
    distances = dijkstra(student.residence, graph).first;
    previous = dijkstra(student.residence, graph).second;

    unordered_set<int> nodes;
    nodes.insert(student.residence);

    for (const string& code : student.classes) {
        if (classes.find(code) != classes.end()) {
            int classLoc = classes[code].locationID;

            int current = classLoc;
            while (current != student.residence) {
                nodes.insert(current);
                current = previous[current];
            }
            nodes.insert(student.residence);
        }
    }

    int cost = MSTCost(nodes);

    cout << "Student Zone Cost For " << student.name << ": " << cost << endl;

    return true;
}

//convert time to mins
int CampusCompass::ConvertToMinutes(const string& timeStr) {
    int hours = stoi(timeStr.substr(0, 2));
    int minutes = stoi(timeStr.substr(3, 2));
    return hours * 60 + minutes;
}

//check if student can make it to all classes on time
bool CampusCompass::HandleVerifyScheduleID(const vector<string>& commands) {
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

    if (student.classes.size() < 2) {
        cout << "unsuccessful" << endl;
        return false;
    }

    vector<ClassInfo> classInfos;
    for (size_t i = 0; i < student.classes.size(); i++) {
        string code = student.classes[i];
        if (classes.find(code) != classes.end()) {
            classInfos.push_back(classes[code]);
        }
    }

    for (size_t i = 0; i < classInfos.size(); i++) {
        for (size_t j = i + 1; j < classInfos.size(); j++) {
            int time_i = ConvertToMinutes(classInfos[i].start);
            int time_j = ConvertToMinutes(classInfos[j].start);
            if (time_i > time_j) {
                ClassInfo temp = classInfos[i];
                classInfos[i] = classInfos[j];
                classInfos[j] = temp;
            }
        }
    }

    cout << "Schedule Check for " << student.name << ": " << endl;
    for (size_t i = 0; i < classInfos.size() - 1; i++) {
        int from = classInfos[i].locationID;
        int to = classInfos[i + 1].locationID;

        unordered_map<int,int> dist = dijkstra(from, graph).first;

        int travel = INT_MAX;
        if (dist.find(to) != dist.end()) {
            travel = dist[to];
        }

        int time = ConvertToMinutes(classInfos[i + 1].start) - ConvertToMinutes(classInfos[i].end);
        
        cout << classInfos[i].code << " - " << classInfos[i + 1].code << " ";
        if (travel <= time) {
            cout << "Can make it!" << endl;
        } else {
            cout << "Cannot make it!" << endl;
        }
    }

    return true;
}