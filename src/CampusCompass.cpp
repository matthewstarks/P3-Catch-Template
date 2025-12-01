#include "CampusCompass.h"
#include <iostream>
#include <sstream>
#include <regex>
#include <fstream>

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
        graph[v].push_back({u, w, true}); // undirected

        idToName[u] = name1;
        idToName[v] = name2;
    }
    return true;
}

bool CampusCompass::LoadClasses(const string &filepath) {
    ifstream file(filepath);
    if (!file.is_open()) return false;

    string line;
    getline(file, line); // skip header

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
    cout << "successful" << endl;
    return true;
}

bool CampusCompass::HandleReplaceClass(const vector<string>& commands) {
    cout << "successful" << endl;
    return true;
}

bool CampusCompass::HandleRemoveClassGlobal(const vector<string>& commands) {
    cout << 0 << endl;
    return true;
}

bool CampusCompass::HandleToggleEdges(const vector<string>& commands) {
    cout << "successful" << endl;
    return true;
}

bool CampusCompass::HandleCheckEdgeStatus(const vector<string>& commands) {
    cout << "open" << endl;
    return true;
}

bool CampusCompass::HandleIsConnected(const vector<string>& commands) {
    cout << "successful" << endl;
    return true;
}

bool CampusCompass::HandlePrintShortestEdges(const vector<string>& commands) {
    cout << "Name: test" << endl;
    return true;
}

bool CampusCompass::HandlePrintStudentZone(const vector<string>& commands) {
    cout << "Student Zone Cost For X: 0" << endl;
    return true;
}
