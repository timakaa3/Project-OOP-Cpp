#include "TaskManager.h"
#include <iostream>
#include <algorithm>

TaskManager::TaskManager(const std::string& path)
    : archive(7), fileManager(path) {
    projects = fileManager.load();
}

TaskManager::~TaskManager() { for (Project* p : projects) delete p; }

Project* TaskManager::selectProject() {
    if (projects.empty()) { std::cout << "Няма проекти.\n"; return nullptr; }
    std::cout << "\nПроекти:\n";
    for (int i = 0; i < (int)projects.size(); ++i)
        std::cout << "  " << i+1 << ". " << projects[i]->getName() << "\n";
    int idx = readInt("Избери проект: ", 1, (int)projects.size());
    return projects[idx-1];
}

Task* TaskManager::selectTask(Project* p) {
    const auto& tasks = p->getTasks();
    if (tasks.empty()) { std::cout << "Няма задачи.\n"; return nullptr; }
    std::cout << "\nЗадачи:\n";
    for (int i = 0; i < (int)tasks.size(); ++i)
        std::cout << "  " << i+1 << ". " << tasks[i]->toString() << "\n";
    int idx = readInt("Избери задача: ", 1, (int)tasks.size());
    return tasks[idx-1];
}

void TaskManager::menuProjects() {
    std::cout << "\n1.Нов проект  2.Изтрий  3.Покажи всички  0.Назад\n";
    int ch = readInt("Избор: ", 0, 3);
    if (ch == 1) {
        std::string id   = "p" + std::to_string(projects.size()+1);
        std::string name = readLine("Име: ");
        std::string desc = readLine("Описание: ");
        projects.push_back(new Project(id, name, desc));
        std::cout << "Проектът е създаден.\n";
    } else if (ch == 2) {
        Project* p = selectProject();
        if (!p) return;
        projects.erase(std::remove(projects.begin(), projects.end(), p), projects.end());
        delete p;
        std::cout << "Изтрит.\n";
    } else if (ch == 3) {
        for (const Project* p : projects) {
            auto s = p->getStats();
            std::cout << "\n[" << p->getName() << "] общо:" << s.total
                      << " завършени:" << s.done << " (" << (int)s.percentDone << "%)"
                      << " просрочени:" << s.overdue << "\n";
            for (const Task* t : p->getTasksByPriority())
                std::cout << "  " << t->toString() << "\n";
        }
    }
}

void TaskManager::menuTasks(Project* p) {
    std::cout << "\n1.Нова задача  2.Повтаряща се  3.Промени статус  "
                 "4.Изтрий  5.Добави таг  6.Редактирай  0.Назад\n";
    int ch = readInt("Избор: ", 0, 6);
    if (ch == 1 || ch == 2) {
        std::string id    = "t" + std::to_string(p->getTasks().size()+1) + "_" + p->getId();
        std::string title = readLine("Заглавие: ");
        std::string desc  = readLine("Описание: ");
        std::string date  = readLine("Краен срок (YYYY-MM-DD): ");
        int prio          = readInt("Приоритет (1-10): ", 1, 10);
        std::time_t due   = parseDate(date);
        if (ch == 2) {
            int rec = readInt("Повторение (дни): ", 1, 365);
            p->addTask(new RecurringTask(id, title, desc, due, prio, rec));
        } else {
            p->addTask(new Task(id, title, desc, due, prio));
        }
        std::cout << "Добавена.\n";
    } else if (ch == 3) {
        Task* t = selectTask(p);
        if (!t) return;
        std::cout << "1.TODO  2.IN_PROGRESS  3.DONE\n";
        int s = readInt("Нов статус: ", 1, 3);
        t->setStatus(static_cast<Status>(s-1));
        std::cout << "Обновено.\n";
    } else if (ch == 4) {
        Task* t = selectTask(p);
        if (!t) return;
        p->removeTask(t->getId());
        std::cout << "Изтрита.\n";
    } else if (ch == 5) {
        Task* t = selectTask(p);
        if (!t) return;
        t->addTag(readLine("Таг: "));
        std::cout << "Тагът е добавен.\n";
    } else if (ch == 6) {
        Task* t = selectTask(p);
        if (!t) return;
        std::cout << "1.Заглавие  2.Описание  3.Краен срок  4.Приоритет\n";
        int field = readInt("Поле: ", 1, 4);
        if (field == 1)      { t->setTitle(readLine("Ново заглавие: ")); }
        else if (field == 2) { t->setDescription(readLine("Ново описание: ")); }
        else if (field == 3) { t->setDueDate(parseDate(readLine("Нов краен срок (YYYY-MM-DD): "))); }
        else if (field == 4) { t->setPriority(readInt("Нов приоритет (1-10): ", 1, 10)); }
        std::cout << "Задачата е обновена.\n";
    }
}

void TaskManager::menuSearch() {
    std::cout << "\n=== Търсене и филтриране ===\n";
    std::string kw = readLine("Ключова дума (Enter=пропусни): ");
    int prioMin    = readInt("Мин. приоритет (1-10): ", 1, 10);
    int prioMax    = readInt("Макс. приоритет (1-10): ", prioMin, 10);

    std::cout << "Статус: 0=Всички  1=TODO  2=IN_PROGRESS  3=DONE\n";
    int statusChoice = readInt("Избор: ", 0, 3);
    int mask = 7;
    if (statusChoice == 1)      mask = 1;
    else if (statusChoice == 2) mask = 2;
    else if (statusChoice == 3) mask = 4;

    TaskFilter f;
    f.setKeyword(kw).setPriorityRange(prioMin, prioMax).setStatusMask(mask);

    auto res = searchTasks(f);
    std::cout << "\nНамерени " << res.size() << " задачи:\n";
    for (const Task* t : res) std::cout << "  " << t->toString() << "\n";
    if (res.empty()) std::cout << "  Няма съвпадения.\n";
}

void TaskManager::menuStats() {
    Project* p = selectProject();
    if (!p) return;
    auto s = p->getStats();
    std::cout << "\n=== Статистика [" << p->getName() << "] ===\n"
              << "  Общо:            " << s.total        << "\n"
              << "  Завършени:       " << s.done << " (" << (int)s.percentDone << "%)\n"
              << "  Просрочени:      " << s.overdue      << "\n"
              << "  Средно (часове): " << s.avgSecondsLeft / 3600.0 << "\n";
}

std::vector<Task*> TaskManager::searchTasks(const TaskFilter& f) const {
    std::vector<Task*> res;
    for (const Project* p : projects)
        for (Task* t : p->getTasks())
            if (f.matches(*t)) res.push_back(t);
    return res;
}

void TaskManager::showReminders() const {
    std::cout << "\n=== Наближаващи дедлайни (до 48ч) ===\n";
    bool any = false;
    for (const Project* p : projects)
        for (const Task* t : p->getTasks())
            if (t->isDueWithin48Hours()) {
                std::cout << "  [" << p->getName() << "] " << t->toString() << "\n";
                any = true;
            }
    if (!any) std::cout << "  Няма.\n";
}

void TaskManager::autoArchive() {
    for (Project* p : projects) {
        std::vector<std::string> toRemove;
        for (Task* t : p->getTasks()) {
            if (archive.archiveIfDone(t)) {
                if (auto* rt = dynamic_cast<RecurringTask*>(t))
                    p->addTask(rt->clone());
                toRemove.push_back(t->getId());
            }
        }
        for (const auto& id : toRemove) p->removeTask(id);
    }
}

void TaskManager::run() {
    showReminders();
    while (true) {
        std::cout << "\n╔══════════════════════════════╗\n"
                  << "║   МЕНИДЖЪР НА ЗАДАЧИ v2.0    ║\n"
                  << "╠══════════════════════════════╣\n"
                  << "║ 1. Проекти                   ║\n"
                  << "║ 2. Задачи                    ║\n"
                  << "║ 3. Търсене & Филтриране      ║\n"
                  << "║ 4. Статистика                ║\n"
                  << "║ 5. Архив                     ║\n"
                  << "║ 6. Запази                    ║\n"
                  << "║ 0. Изход                     ║\n"
                  << "╚══════════════════════════════╝\n";
        int ch = readInt("Избор: ", 0, 6);
        switch (ch) {
            case 1: menuProjects(); break;
            case 2: { Project* p = selectProject(); if (p) menuTasks(p); break; }
            case 3: menuSearch(); break;
            case 4: menuStats(); break;
            case 5: autoArchive(); archive.printAll(); break;
            case 6: fileManager.save(projects); std::cout << "Запазено.\n"; break;
            case 0:
                fileManager.save(projects);
                std::cout << "Довиждане!\n";
                return;
        }
    }
}
