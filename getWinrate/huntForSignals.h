#ifndef GETWINRATE_HUNTFORSIGNALS_H
#define GETWINRATE_HUNTFORSIGNALS_H

#include <string>
#include "sqlite3.h"
void lookForSignals (std::string pathToFileWithLinks, std::string jsonFileName);
int findNewRandomNumber(sqlite3* currentDb, std::string summaryLast, std::string tableName, int upperBound);

#endif //GETWINRATE_HUNTFORSIGNALS_H
