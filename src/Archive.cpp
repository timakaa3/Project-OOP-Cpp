#include "Archive.h"
#include <iostream>
#include <ctime>

Archive::Archive(int days) : retentionDays(days) {}

Archive::~Archive() { for (Task* t : archivedTasks) delete t; }

bool Archive::archiveIfDone(Task* t) {
    if (t->getStatus() != Status::DONE) return false;
    std::time_t threshold = std::time(nullptr) - retentionDays * 86400LL;
    if (t->getDueDate() < threshold) {
        archivedTasks.push_back(t->clone());
        return true;
    }
    return false;
}

void Archive::printAll() const {
    std::cout << "=== Архив (" << archivedTasks.size() << " задачи) ===\n";
    for (const Task* t : archivedTasks) std::cout << "  " << t->toString() << "\n";
    if (archivedTasks.empty()) std::cout << "  Архивът е празен.\n";
}
