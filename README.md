# Мениджър на задачи и проекти

Конзолно приложение на C++17 за управление на задачи и проекти.

## Функционалности (10/10)

| # | Функционалност | Клас |
|---|---|---|
| 1 | Управление на задачи (CRUD) | `Task`, `TaskManager` |
| 2 | Управление на проекти | `Project`, `TaskManager` |
| 3 | Приоритизиране с тегла (1–10) | `Task`, `Project::getTasksByPriority()` |
| 4 | Статистика по проект | `Project::getStats()` → `ProjectStats` |
| 5 | Автоматично архивиране | `Archive`, `TaskManager::autoArchive()` |
| 6 | Търсене и филтриране | `TaskFilter` (Specification pattern) |
| 7 | Цветово маркиране с тагове | `Task::addTag()` |
| 8 | Запазване и зареждане от файл | `FileManager` (CSV формат) |
| 9 | Напомняния за наближаващи дедлайни | `Task::isDueWithin48Hours()` |
| 10 | Повтарящи се задачи | `RecurringTask` (наследява `Task`) |

## Файлова структура
project/
├── include/
│   ├── common.h
│   ├── Task.h
│   ├── TaskFilter.h
│   ├── Project.h
│   ├── Archive.h
│   ├── FileManager.h
│   └── TaskManager.h
├── src/
│   ├── common.cpp
│   ├── Task.cpp
│   ├── TaskFilter.cpp
│   ├── Project.cpp
│   ├── Archive.cpp
│   ├── FileManager.cpp
│   ├── TaskManager.cpp
│   └── main.cpp
└── Makefile

## Компилация и стартиране

```bash
# Linux / Mac
make
./task_manager

# Windows
chcp 65001
g++ -std=c++17 -Iinclude src/common.cpp src/Task.cpp src/TaskFilter.cpp src/Project.cpp src/Archive.cpp src/FileManager.cpp src/TaskManager.cpp src/main.cpp -o task_manager
./task_manager
```

## Дизайнерски решения

**Решение 1 — Task е базов клас, RecurringTask го наследява**
Вместо едно поле `isRecurring` в Task, RecurringTask е отделен наследник. Полиморфизмът позволява на TaskManager да работи с `Task*` указатели без да знае дали задачата е повтаряща се.

**Решение 2 — TaskFilter е отделен клас (Specification pattern)**
Вместо метод с 4 параметъра, TaskFilter е обект с метод `matches(const Task&)`. Добавянето на нов критерий изисква промяна само в TaskFilter.

**Решение 3 — Полетата в Task са private**
Setter-ите съдържат валидация — `setPriority()` гарантира стойност в [1,10], `setTitle()` отхвърля празен низ.

**Решение 4 — FileManager е отделен клас (Single Responsibility)**
TaskManager съдържа само бизнес логика. FileManager може лесно да смени формата от CSV към JSON без да се докосва TaskManager.
