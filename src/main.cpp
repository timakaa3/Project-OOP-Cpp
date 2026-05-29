#include "TaskManager.h"

int main(int argc, char* argv[]) {
    std::string dataFile = argc >= 2 ? argv[1] : "tasks.csv";
    TaskManager manager(dataFile);
    manager.run();
    return 0;
}
