#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <limits>
#include <climits>
#include <ctime>

enum class Status { TODO, IN_PROGRESS, DONE };

std::string statusToString(Status s);
Status stringToStatus(const std::string& s);

std::vector<std::string> splitCSV(const std::string& s, char d = ',');
void clearLine();
int readInt(const std::string& prompt, int lo = INT_MIN, int hi = INT_MAX);
std::string readLine(const std::string& prompt);
std::time_t parseDate(const std::string& s);
