#pragma once
#include "Task.h"
#include <vector>

class Archive {
private:
    std::vector<Task*> archivedTasks;
    int retentionDays;

public:
    explicit Archive(int days = 7);
    ~Archive();

    bool archiveIfDone(Task* t);
    const std::vector<Task*>& getAll() const { return archivedTasks; }
    void printAll() const;
};
