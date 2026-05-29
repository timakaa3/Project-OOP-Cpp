#include "TaskFilter.h"
#include <algorithm>
#include <iostream>

bool TaskFilter::matches(const Task& task) const {
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

std::ostream& operator<<(std::ostream& os, const TaskFilter& f) {
    os << "Filter[kw='" << f.keyword << "' p=" << f.priorityMin << "-" << f.priorityMax << "]";
    return os;
}
