//
// Created by c on 14.09.2020.
//

#ifndef COLLECTSTATS_COMMANDSFORDB_H
#define COLLECTSTATS_COMMANDSFORDB_H

#include "sqlite3.h"
#include <string>
#include <vector>

int countLosesForPairTfAndCertain(sqlite3* pointerOnDb, char* messageError,
                           std::string& pairname, std::string tf = "-1",
                           int certain = -1, std::string forDay = "-1");
int countAllStakesOfPairTfAndCertain(sqlite3* pointerOnDb, char* messageError,
                                     std::string& pairname, std::string tf = "-1",
                                     int certain = -1, std::string forDay = "-1");
std::vector<int> findPossibleCertainValues(sqlite3* pointerOnDb, char* messageError);
std::vector<std::string> findPossibleTFs(sqlite3* pointerOnDb, char* messageError);

#endif //COLLECTSTATS_COMMANDSFORDB_H
