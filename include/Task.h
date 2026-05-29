#pragma once
#include "common.h"
#include <string>
#include <vector>
#include <ctime>
#include <stdexcept>
#include <sstream>
#include <algorithm>

class Task {
protected:
    std::string taskId, title, description;
    std::time_t dueDate;
    int priority;
    Status status;
    std::vector<std::string> tags;

public:
    Task(const std::string& id, const std::string& title,
         const std::string& desc, std::time_t due, int prio);

    virtual ~Task() = default;

    std::string getId()          const { return taskId; }
    std::string getTitle()       const { return title; }
    std::string getDescription() const { return description; }
    std::time_t getDueDate()     const { return dueDate; }
    int         getPriority()    const { return priority; }
    Status      getStatus()      const { return status; }
    const std::vector<std::string>& getTags() const { return tags; }

    void setTitle(const std::string& t);
    void setDescription(const std::string& d) { description = d; }
    void setDueDate(std::time_t d)             { dueDate = d; }
    void setPriority(int p);
    void setStatus(Status s) { status = s; }
    void addTag(const std::string& tag) { tags.push_back(tag); }

    bool isOverdue() const;
    bool isDueWithin48Hours() const;

    virtual std::string toString() const;
    virtual Task* clone() const { return new Task(*this); }
    virtual std::string toCSV() const;
};

class RecurringTask : public Task {
private:
    int recurrenceDays;

public:
    RecurringTask(const std::string& id, const std::string& title,
                  const std::string& desc, std::time_t due, int prio, int days);

    int         getRecurrenceDays() const { return recurrenceDays; }
    std::time_t getNextOccurrence() const { return getDueDate() + recurrenceDays * 86400LL; }

    std::string toString() const override;
    Task* clone() const override;
    std::string toCSV() const override;
};
