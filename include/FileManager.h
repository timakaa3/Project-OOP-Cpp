#pragma once
#include "Project.h"
#include <string>
#include <vector>
#include <fstream>
#include <iostream>

class FileManager {
private:
    std::string filePath;
    static Task* taskFromLine(const std::string& line);

public:
    explicit FileManager(const std::string& path);

    bool save(const std::vector<Project*>& projects) const;
    std::vector<Project*> load() const;

    friend std::ostream& operator<<(std::ostream& os, const FileManager& fm);
    friend std::istream& operator>>(std::istream& is, FileManager& fm);
};
