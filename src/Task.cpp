#include "Task.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <algorithm>

Task::Task(const std::string& id, const std::string& t,
           const std::string& desc, std::time_t due, int prio)
    : taskId(id), title(t), description(desc),
      dueDate(due), status(Status::TODO) { setPriority(prio); }

void Task::setTitle(const std::string& t) {
    if (t.empty()) throw std::invalid_argument("Title cannot be empty.");
    title = t;
}

void Task::setPriority(int p) {
    if (p < 1 || p > 10) throw std::invalid_argument("Priority must be 1-10.");
    priority = p;
}

bool Task::isOverdue() const {
    return status != Status::DONE && std::time(nullptr) > dueDate;
}

bool Task::isDueWithin48Hours() const {
    if (status == Status::DONE) return false;
    std::time_t now = std::time(nullptr);
    return dueDate >= now && dueDate <= now + 48 * 3600;
}

std::string Task::toString() const {
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

std::string Task::toCSV() const {
    std::string tagStr;
    for (size_t i = 0; i < tags.size(); ++i) {
        if (i) tagStr += ";";
        tagStr += tags[i];
    }
    return "TASK," + taskId + "," + title + "," + description + "," +
           std::to_string(dueDate) + "," + std::to_string(priority) + "," +
           statusToString(status) + "," + tagStr;
}

// ── RecurringTask ──────────────────────────────────────

RecurringTask::RecurringTask(const std::string& id, const std::string& t,
                             const std::string& desc, std::time_t due, int prio, int days)
    : Task(id, t, desc, due, prio), recurrenceDays(days) {}

std::string RecurringTask::toString() const {
    return Task::toString() + " [ПОВТАРЯ се на " + std::to_string(recurrenceDays) + " дни]";
}

Task* RecurringTask::clone() const {
    RecurringTask* next = new RecurringTask(
        getId() + "_next", getTitle(), getDescription(),
        getNextOccurrence(), getPriority(), recurrenceDays);
    for (const auto& tag : getTags()) next->addTag(tag);
    return next;
}

std::string RecurringTask::toCSV() const {
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
