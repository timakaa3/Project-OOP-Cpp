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

// ══════════════════════════════════════════════════════
//  CLASS: Task
// ══════════════════════════════════════════════════════
 
class Task {
protected:
    std::string taskId, title, description;
    std::time_t dueDate;
    int priority;
    Status status;
    std::vector<std::string> tags;
 
public:
    Task(const std::string& id, const std::string& title,
         const std::string& desc, std::time_t due, int prio)
        : taskId(id), title(title), description(desc),
          dueDate(due), status(Status::TODO) { setPriority(prio); }
 
    virtual ~Task() = default;
 
    // Getters
    std::string getId()          const { return taskId; }
    std::string getTitle()       const { return title; }
    std::string getDescription() const { return description; }
    std::time_t getDueDate()     const { return dueDate; }
    int         getPriority()    const { return priority; }
    Status      getStatus()      const { return status; }
    const std::vector<std::string>& getTags() const { return tags; }
 
    // Setters with validation
    void setTitle(const std::string& t) {
        if (t.empty()) throw std::invalid_argument("Title cannot be empty.");
        title = t;
    }
    void setDescription(const std::string& d) { description = d; }
    void setDueDate(std::time_t d)             { dueDate = d; }
    void setPriority(int p) {
        if (p < 1 || p > 10) throw std::invalid_argument("Priority must be 1-10.");
        priority = p;
    }
    void setStatus(Status s) { status = s; }
    void addTag(const std::string& tag) { tags.push_back(tag); }
 
    bool isOverdue() const {
        return status != Status::DONE && std::time(nullptr) > dueDate;
    }
    bool isDueWithin48Hours() const {
        if (status == Status::DONE) return false;
        std::time_t now = std::time(nullptr);
        return dueDate >= now && dueDate <= now + 48 * 3600;
    }
 
    virtual std::string toString() const {
        char buf[20];
        std::tm* tm_info = std::localtime(&dueDate);
        std::strftime(buf, sizeof(buf), "%Y-%m-%d", tm_info);
        std::ostringstream oss;
        oss << "[" << taskId << "] " << title
            << " | P:" << priority
            << " | " << statusToString(status)
            << " | " << buf;
        if (!tags.empty()) {
            oss << " | #";
            for (size_t i = 0; i < tags.size(); ++i) {
                if (i) oss << " #";
                oss << tags[i];
            }
        }
        if (isOverdue()) oss << " *** ПРОСРОЧЕНА ***";
        return oss.str();
    }
 
    virtual Task* clone() const { return new Task(*this); }
 
    virtual std::string toCSV() const {
        std::string tagStr;
        for (size_t i = 0; i < tags.size(); ++i) {
            if (i) tagStr += ";";
            tagStr += tags[i];
        }
        return "TASK," + taskId + "," + title + "," + description + "," +
               std::to_string(dueDate) + "," + std::to_string(priority) + "," +
               statusToString(status) + "," + tagStr;
    }
};
