#include <iostream>

#include "CampusCompass.h"

using namespace std;

int main() {
    // initialize your main project object
    CampusCompass compass;

    // ingest CSV data
    bool loaded = compass.ParseCSV("data/edges.csv", "data/classes.csv") || compass.ParseCSV("../data/edges.csv", "../data/classes.csv");

    // the below is example code for parsing commandline input
    int no_of_lines;
    string command;
    cin >> no_of_lines;
    cin.ignore(); // ignore newline that first cin left over
    for (int i = 0; i < no_of_lines; i++) {
        getline(cin, command);

        if (command.empty() || command.find_first_not_of(" \t\r\n") == string::npos) // Did this because my commands were not working correctly in gradescrope this sends it back one iteration. Same logic is placed in cpp.
        {
            i--;
            continue;
        }

        // parse your commands however you see fit
        compass.ParseCommand(command);
    }
}
