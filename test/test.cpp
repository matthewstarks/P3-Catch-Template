// #define CATCH_CONFIG_MAIN
// #include "catch/catch_amalgamated.hpp"
// #include "../src/CampusCompass.h"
// #include <iostream>
// #include <string>

// using namespace std;

// string captureOutput(CampusCompass &c, const string &command) {
//     stringstream buffer;
//     streambuf* old = cout.rdbuf(buffer.rdbuf());
//     c.ParseCommand(command);
//     cout.rdbuf(old);
//     return buffer.str();
// }

// TEST_CASE("incorrect commands", "[commands]") {
//     CampusCompass c;
//     c.ParseCSV("edges.csv", "classes.csv");

//     REQUIRE(captureOutput(c, R"(insert "A11y" 45679999 1 1 COP3530)") == "unsuccessful\n");
//     REQUIRE(captureOutput(c, R"(insert "Alice" 1234 1 1 COP3530)") == "unsuccessful\n");
//     REQUIRE(captureOutput(c, R"(insert "Alice" 10000001 1 1 COP35X0)") == "unsuccessful\n");
//     REQUIRE(captureOutput(c, R"(remove 99999999)") == "unsuccessful\n");

//     c.ParseCommand(R"(insert "Bob" 10000002 1 1 COP3502)");
//     REQUIRE(captureOutput(c, R"(dropClass 10000002 MAC2311)") == "unsuccessful\n");
// }

// TEST_CASE("edge cases", "[edge_cases]") {
//     CampusCompass c;
//     c.ParseCSV("edges.csv", "classes.csv");

//     REQUIRE(captureOutput(c, R"(remove 10000003)") == "unsuccessful\n");
//     REQUIRE(captureOutput(c, R"(toggleEdgesClosure 2 1 2 3)") == "unsuccessful\n");
//     REQUIRE(captureOutput(c, R"(checkEdgeStatus 1 99)") == "DNE\n");
// }

// TEST_CASE("class management commands", "[classes]") {
//     CampusCompass c;
//     c.ParseCSV("edges.csv", "classes.csv");

//     REQUIRE(captureOutput(c, R"(insert "Alice" 10000001 1 2 COP3502 MAC2311)") == "successful\n");
//     REQUIRE(captureOutput(c, R"(insert "Bob" 10000002 1 1 COP3502)") == "successful\n");

//     REQUIRE(captureOutput(c, R"(dropClass 10000001 MAC2311)") == "successful\n");
//     REQUIRE(captureOutput(c, R"(replaceClass 10000002 COP3502 MAC2311)") == "successful\n");
//     REQUIRE(captureOutput(c, R"(remove 10000001)") == "successful\n");
//     REQUIRE(captureOutput(c, R"(removeClass MAC2311)") == "1\n");
// }

// TEST_CASE("printShortestEdges with toggled edges", "[shortestEdges]") {
//     CampusCompass c;
//     c.ParseCSV("edges.csv", "classes.csv");

//     REQUIRE(captureOutput(c, R"(insert "Alice" 10000001 1 2 COP3502 COP3503)") == "successful\n");

//     REQUIRE(captureOutput(c, R"(toggleEdgesClosure 3 1 2 1 50 1 4)") == "successful\n");

//     string output2 = captureOutput(c, R"(printShortestEdges 10000001)");
//     REQUIRE(output2 == 
//         "Name: Alice\n"
//         "COP3502 | Total Time: -1\n"
//         "COP3503 | Total Time: -1\n"
//     );
// }