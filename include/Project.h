#pragma once
#include "Task.h"
#include <string>
#include <vector>
#include <algorithm>

struct ProjectStats {
    int total, done, overdue;
    double percentDone, avgSecondsLeft;
};

class Project {
private:
    std::string projectId, name, description;
    std::vector<Task*> tasks;

public:
    Project(const std::string& id, const std::string& name, const std::string& desc = "");
    ~Project();

    std::string getId()   const { return projectId; }
    std::string getName() const { return name; }
    std::string getDesc() const { return description; }
    const std::vector<Task*>& getTasks() const { return tasks; }

    void  addTask(Task* t);
    bool  removeTask(const std::string& id);
    Task* findTask(const std::string& id);

    ProjectStats       getStats()           const;
    std::vector<Task*> getTasksByPriority() const;
};
