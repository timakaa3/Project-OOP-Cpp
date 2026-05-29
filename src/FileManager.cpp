#include "FileManager.h"
#include <fstream>
#include <sstream>
#include <iostream>

FileManager::FileManager(const std::string& path) : filePath(path) {}

Task* FileManager::taskFromLine(const std::string& line) {
    auto c = splitCSV(line);
    if (c.size() < 8) return nullptr;
    std::time_t due = (std::time_t)std::stoll(c[4]);
    int prio        = std::stoi(c[5]);
    Status st       = stringToStatus(c[6]);
    auto tagList    = splitCSV(c[7], ';');

    Task* t = nullptr;
    if (c[0] == "RECURRING" && c.size() >= 9)
        t = new RecurringTask(c[1], c[2], c[3], due, prio, std::stoi(c[8]));
    else
        t = new Task(c[1], c[2], c[3], due, prio);

    t->setStatus(st);
    for (const auto& tag : tagList) if (!tag.empty()) t->addTag(tag);
    return t;
}

bool FileManager::save(const std::vector<Project*>& projects) const {
    std::ofstream ofs(filePath);
    if (!ofs) { std::cerr << "Грешка при запис: " << filePath << "\n"; return false; }
    for (const Project* p : projects) {
        ofs << "PROJECT," << p->getId() << "," << p->getName() << "," << p->getDesc() << "\n";
        for (const Task* t : p->getTasks()) ofs << t->toCSV() << "\n";
    }
    ofs << "END\n";
    return true;
}

std::vector<Project*> FileManager::load() const {
    std::vector<Project*> projects;
    std::ifstream ifs(filePath);
    if (!ifs) return projects;
    Project* cur = nullptr;
    std::string line;
    while (std::getline(ifs, line)) {
        if (line.empty() || line == "END") continue;
        if (line.substr(0, 8) == "PROJECT,") {
            auto c = splitCSV(line);
            if (c.size() >= 3) {
                cur = new Project(c[1], c[2], c.size() >= 4 ? c[3] : "");
                projects.push_back(cur);
            }
        } else if (cur) {
            Task* t = taskFromLine(line);
            if (t) cur->addTask(t);
        }
    }
    return projects;
}

std::ostream& operator<<(std::ostream& os, const FileManager& fm) {
    os << "FileManager[" << fm.filePath << "]"; return os;
}

std::istream& operator>>(std::istream& is, FileManager& fm) {
    std::cout << "Нов файл: "; is >> fm.filePath; return is;
}
