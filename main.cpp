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

// ══════════════════════════════════════════════════════
//  CLASS: RecurringTask  (наследява Task)
// ══════════════════════════════════════════════════════
 
class RecurringTask : public Task {
private:
    int recurrenceDays;
 
public:
    RecurringTask(const std::string& id, const std::string& title,
                  const std::string& desc, std::time_t due, int prio, int days)
        : Task(id, title, desc, due, prio), recurrenceDays(days) {}
 
    int         getRecurrenceDays()   const { return recurrenceDays; }
    std::time_t getNextOccurrence()   const { return getDueDate() + recurrenceDays * 86400LL; }
 
    std::string toString() const override {
        return Task::toString() + " [ПОВТАРЯ се на " + std::to_string(recurrenceDays) + " дни]";
    }
 
    Task* clone() const override {
        RecurringTask* next = new RecurringTask(
            getId() + "_next", getTitle(), getDescription(),
            getNextOccurrence(), getPriority(), recurrenceDays);
        for (const auto& tag : getTags()) next->addTag(tag);
        return next;
    }
 
    std::string toCSV() const override {
        std::string tagStr;
        for (size_t i = 0; i < getTags().size(); ++i) {
            if (i) tagStr += ";";
            tagStr += getTags()[i];
        }
        return "RECURRING," + getId() + "," + getTitle() + "," + getDescription() + "," +
               std::to_string(getDueDate()) + "," + std::to_string(getPriority()) + "," +
               statusToString(getStatus()) + "," + tagStr + "," +
               std::to_string(recurrenceDays);
    }
};

// ══════════════════════════════════════════════════════
//  CLASS: TaskFilter  (Specification pattern)
// ══════════════════════════════════════════════════════
 
class TaskFilter {
private:
    std::string keyword;
    int priorityMin = 1, priorityMax = 10;
    int statusMask  = 7; // bit0=TODO, bit1=IN_PROGRESS, bit2=DONE  (7 = all)
    std::time_t deadlineBefore = 0;
 
public:
    TaskFilter& setKeyword(const std::string& kw)          { keyword = kw; return *this; }
    TaskFilter& setPriorityRange(int lo, int hi)           { priorityMin = lo; priorityMax = hi; return *this; }
    TaskFilter& setStatusMask(int mask)                    { statusMask = mask; return *this; }
    TaskFilter& setDeadlineBefore(std::time_t t)           { deadlineBefore = t; return *this; }
 
    bool matches(const Task& task) const {
        if (!keyword.empty()) {
            std::string kl = keyword, tl = task.getTitle(), dl = task.getDescription();
            std::transform(kl.begin(), kl.end(), kl.begin(), ::tolower);
            std::transform(tl.begin(), tl.end(), tl.begin(), ::tolower);
            std::transform(dl.begin(), dl.end(), dl.begin(), ::tolower);
            if (tl.find(kl) == std::string::npos && dl.find(kl) == std::string::npos)
                return false;
        }
        if (!(statusMask & (1 << (int)task.getStatus()))) return false;
        if (task.getPriority() < priorityMin || task.getPriority() > priorityMax) return false;
        if (deadlineBefore && task.getDueDate() > deadlineBefore) return false;
        return true;
    }
 
    friend std::ostream& operator<<(std::ostream& os, const TaskFilter& f) {
        os << "Filter[kw='" << f.keyword << "' p=" << f.priorityMin << "-" << f.priorityMax << "]";
        return os;
    }
};

// ══════════════════════════════════════════════════════
//  STRUCT: ProjectStats
// ══════════════════════════════════════════════════════
 
struct ProjectStats {
    int total, done, overdue;
    double percentDone, avgSecondsLeft;
};
 
// ══════════════════════════════════════════════════════
//  CLASS: Project
// ══════════════════════════════════════════════════════
 
class Project {
private:
    std::string projectId, name, description;
    std::vector<Task*> tasks;
 
public:
    Project(const std::string& id, const std::string& name, const std::string& desc = "")
        : projectId(id), name(name), description(desc) {}
 
    ~Project() { for (Task* t : tasks) delete t; }
 
    std::string getId()   const { return projectId; }
    std::string getName() const { return name; }
    std::string getDesc() const { return description; }
    const std::vector<Task*>& getTasks() const { return tasks; }
 
    void addTask(Task* t) { tasks.push_back(t); }
 
    bool removeTask(const std::string& id) {
        for (auto it = tasks.begin(); it != tasks.end(); ++it) {
            if ((*it)->getId() == id) { delete *it; tasks.erase(it); return true; }
        }
        return false;
    }
 
    Task* findTask(const std::string& id) {
        for (Task* t : tasks) if (t->getId() == id) return t;
        return nullptr;
    }
 
    ProjectStats getStats() const {
        ProjectStats s{};
        s.total = (int)tasks.size();
        double totalLeft = 0;
        std::time_t now = std::time(nullptr);
        for (const Task* t : tasks) {
            if (t->getStatus() == Status::DONE) ++s.done;
            if (t->isOverdue()) ++s.overdue;
            totalLeft += (double)(t->getDueDate() - now);
        }
        s.percentDone    = s.total ? 100.0 * s.done / s.total : 0;
        s.avgSecondsLeft = s.total ? totalLeft / s.total : 0;
        return s;
    }
 
    std::vector<Task*> getTasksByPriority() const {
        std::vector<Task*> sorted = tasks;
        std::sort(sorted.begin(), sorted.end(),
            [](Task* a, Task* b) { return a->getPriority() > b->getPriority(); });
        return sorted;
    }
};
