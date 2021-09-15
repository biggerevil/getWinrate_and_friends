#include "copyDatabase.h"
#include <filesystem>

void copyDatabase() {
    std::filesystem::copy(pathToResultsDb, pathToCopiedDb);
}

void deleteCopyOfDatabase() {
    std::filesystem::remove(pathToCopiedDb);
}

