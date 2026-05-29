#include "Project.h"
#include <ctime>
#include <algorithm>

Project::Project(const std::string& id, const std::string& n, const std::string& desc)
    : projectId(id), name(n), description(desc) {}

Project::~Project() { for (Task* t : tasks) delete t; }

void Project::addTask(Task* t) { tasks.push_back(t); }

bool Project::removeTask(const std::string& id) {
    for (auto it = tasks.begin(); it != tasks.end(); ++it) {
        if ((*it)->getId() == id) { delete *it; tasks.erase(it); return true; }
    }
    return false;
}

Task* Project::findTask(const std::string& id) {
    for (Task* t : tasks) if (t->getId() == id) return t;
    return nullptr;
}

ProjectStats Project::getStats() const {
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

std::vector<Task*> Project::getTasksByPriority() const {
    std::vector<Task*> sorted = tasks;
    std::sort(sorted.begin(), sorted.end(),
        [](Task* a, Task* b) { return a->getPriority() > b->getPriority(); });
    return sorted;
}
