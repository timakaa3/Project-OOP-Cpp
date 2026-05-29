#pragma once
#include "Task.h"
#include <string>
#include <ctime>
#include <ostream>

class TaskFilter {
private:
    std::string keyword;
    int priorityMin = 1, priorityMax = 10;
    int statusMask  = 7; // bit0=TODO, bit1=IN_PROGRESS, bit2=DONE
    std::time_t deadlineBefore = 0;

public:
    TaskFilter& setKeyword(const std::string& kw)   { keyword = kw; return *this; }
    TaskFilter& setPriorityRange(int lo, int hi)     { priorityMin = lo; priorityMax = hi; return *this; }
    TaskFilter& setStatusMask(int mask)              { statusMask = mask; return *this; }
    TaskFilter& setDeadlineBefore(std::time_t t)     { deadlineBefore = t; return *this; }

    bool matches(const Task& task) const;

    friend std::ostream& operator<<(std::ostream& os, const TaskFilter& f);
};
