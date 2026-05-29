#pragma once
#include "Project.h"
#include "Archive.h"
#include "FileManager.h"
#include "TaskFilter.h"
#include <vector>
#include <string>

class TaskManager {
private:
    std::vector<Project*> projects;
    Archive     archive;
    FileManager fileManager;

    Project* selectProject();
    Task*    selectTask(Project* p);

    void menuProjects();
    void menuTasks(Project* p);
    void menuSearch();
    void menuStats();

public:
    explicit TaskManager(const std::string& path = "tasks.csv");
    ~TaskManager();

    std::vector<Task*> searchTasks(const TaskFilter& f) const;
    void showReminders() const;
    void autoArchive();
    void run();
};
