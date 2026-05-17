#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <limits>
#include <climits>
 
// ══════════════════════════════════════════════════════
//  ENUMS & HELPERS
// ══════════════════════════════════════════════════════
 
enum class Status { TODO, IN_PROGRESS, DONE };
 
std::string statusToString(Status s) {
    if (s == Status::IN_PROGRESS) return "IN_PROGRESS";
    if (s == Status::DONE)        return "DONE";
    return "TODO";
}
 
Status stringToStatus(const std::string& s) {
    if (s == "IN_PROGRESS") return Status::IN_PROGRESS;
    if (s == "DONE")        return Status::DONE;
    return Status::TODO;
}
 
static std::vector<std::string> splitCSV(const std::string& s, char d = ',') {
    std::vector<std::string> r;
    std::istringstream ss(s);
    std::string t;
    while (std::getline(ss, t, d)) r.push_back(t);
    return r;
}
 
static void clearLine() {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}
 
static int readInt(const std::string& prompt, int lo = INT_MIN, int hi = INT_MAX) {
    int v;
    while (true) {
        std::cout << prompt;
        if (std::cin >> v && v >= lo && v <= hi) { clearLine(); return v; }
        std::cout << "  Невалиден вход. Опитай пак.\n";
        std::cin.clear(); clearLine();
    }
}
 
static std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string s;
    std::getline(std::cin, s);
    return s;
}
 
static std::time_t parseDate(const std::string& s) {
    std::tm tm{};
    std::istringstream ss(s);
    ss >> std::get_time(&tm, "%Y-%m-%d");
    tm.tm_hour = 23; tm.tm_min = 59; tm.tm_sec = 59;
    return std::mktime(&tm);
}
