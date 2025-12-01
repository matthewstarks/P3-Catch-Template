#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <map>


struct Edge {
    int to;
    int weight;
    bool open = true;
};

struct ClassInfo {
    std::string code;
    int locationID;
    std::string start;
    std::string end;
};

struct Student {
    std::string name;
    std::string id;
    int residence;
    std::vector<std::string> classes;
};

class CampusCompass {
private:
    std::unordered_map<int, std::vector<Edge>> graph; // adjacency list
    std::unordered_map<std::string, ClassInfo> classes;  
    std::unordered_map<int, std::string> idToName;     // locationID name

    std::unordered_map<std::string, Student> students; // UFID student

private:
    // CSV helpers
    bool LoadEdges(const std::string& filepath);
    bool LoadClasses(const std::string& filepath);

    // Command helpers
    bool HandleInsert(const std::vector<std::string>& commands);
    bool HandleRemove(const std::vector<std::string>& commands);
    bool HandleDropClass(const std::vector<std::string>& commands);
    bool HandleReplaceClass(const std::vector<std::string>& commands);
    bool HandleRemoveClassGlobal(const std::vector<std::string>& commands);
    bool HandleToggleEdges(const std::vector<std::string>& commands);
    bool HandleCheckEdgeStatus(const std::vector<std::string>& commands);
    bool HandleIsConnected(const std::vector<std::string>& commands);
    bool HandlePrintShortestEdges(const std::vector<std::string>& commands);
    bool HandlePrintStudentZone(const std::vector<std::string>& commands);

    // Utility helpers
    std::vector<std::string> Commands(const std::string& command);
    bool IsValidUFID(const std::string& id);
    bool IsValidName(const std::string& name);
    bool IsValidClassCode(const std::string& code);

public:
    CampusCompass();
    bool ParseCSV(const std::string &edges_filepath, const std::string &classes_filepath);
    bool ParseCommand(const std::string &command);
};
