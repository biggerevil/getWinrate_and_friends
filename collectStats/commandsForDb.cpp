#include "commandsForDb.h"

#include <sqlite3.h>
#include <iostream>
#include <vector>

static int callbackForGettingOneValue(void* data, int argc, char** argv, char** azColName) {
    int* oneValueToReturn = reinterpret_cast<int*>(data);
    *oneValueToReturn = atoi(argv[0]);
    return 0;
}

int countLosesForPairTfAndCertain(sqlite3* pointerOnDb, char* messageError,
                                  std::string& pairname, std::string tf,
                                  int certain, std::string forDay) {
    std::string sql = "SELECT COUNT(*) FROM RESULTSSTAVKI where PAIRNAME = \"" + pairname + "\" and success = 0";
    if (tf != "-1") {
        sql += " and EXPIREPERIOD=\"" + tf + "\"";
    }
    if (certain != -1) {
        sql += " and CERTAIN=" + std::to_string(certain);
    }
    if (forDay != "-1") {
        // В итогу будет sql +=
        //  and DATEWHENSET>="2020-09-14 00:00" and DATEWHENSET<="2020-09-15 23:59"
        sql += " and DATEWHENSET>=\"" + forDay + " 00:00\" and DATEWHENSET<=\"" +
               forDay + " 23:59\"";
    }
    sql += ";";
    int losesForPair;
    int exit = sqlite3_exec(pointerOnDb, sql.c_str(), callbackForGettingOneValue, &losesForPair, &messageError);
    // TODO: на GBP/USD, tf=5m здесь получается плохой sql запрос, который возвращет 0 (
    //  уже проверил с помощью TablePlus, что дело именно в запросе)
    if (exit != SQLITE_OK) {
        std::cout << "Error trying to exec sql " << sql << ". messageError - "
                  << messageError << ", exit = " << exit << std::endl;
        std::exit(-1);
    }

    return losesForPair;
}

int countAllStakesOfPairTfAndCertain(sqlite3* pointerOnDb, char* messageError,
                                     std::string& pairname, std::string tf,
                                     int certain, std::string forDay) {
    std::string sql = "SELECT COUNT(*) FROM RESULTSSTAVKI where PAIRNAME = \"" + pairname + "\"";
    if (tf != "-1") {
        sql += " and EXPIREPERIOD=\"" + tf + "\"";
    }
    if (certain != -1) {
        sql += " and CERTAIN=" + std::to_string(certain);
    }
    if (forDay != "-1") {
        // В итогу будет sql +=
        //  and DATEWHENSET>="2020-09-14 00:00" and DATEWHENSET<="2020-09-15 23:59"
        sql += " and DATEWHENSET>=\"" + forDay + " 00:00\" and DATEWHENSET<=\"" +
                forDay + " 23:59\"";
    }
    sql += ";";
    int allStakesCount;
    int exit = sqlite3_exec(pointerOnDb, sql.c_str(), callbackForGettingOneValue, &allStakesCount, &messageError);

    if (exit != SQLITE_OK) {
        std::cout << "Error trying to exec sql " << sql << ". messageError - "
                  << messageError << ", exit = " << exit << std::endl;
        std::exit(-1);
    }

    return allStakesCount;
}

// Этот callback вызывается каждый раз, когда находится новое значение certain
static int callbackForFindingCertain(void* data, int argc, char** argv, char** azColName) {

    std::vector<int>* certainValues =
            reinterpret_cast<std::vector<int>*>(data);
    certainValues->push_back(atoi(*argv));

    return 0;
}

std::vector<int> findPossibleCertainValues(sqlite3* pointerOnDb,
                                           char* messageError) {
    std::string sql = "SELECT DISTINCT certain FROM RESULTSSTAVKI ORDER BY CERTAIN";
    std::vector<int> certainValues;
    int exit = sqlite3_exec(pointerOnDb, sql.c_str(),
                            callbackForFindingCertain, &certainValues, &messageError);
    if (exit != SQLITE_OK) {
        std::cout << "Error trying to exec sql " << sql << ". messageError - "
                  << messageError << ", exit = " << exit << std::endl;
        std::exit(-1);
    }
    return certainValues;
}

static int callbackForFindingTFValues(void* data, int argc, char** argv, char** azColName) {

    std::vector<std::string>* vectorWithTFs =
            reinterpret_cast<std::vector<std::string>*>(data);
    vectorWithTFs->push_back(*argv);
    return 0;
}

std::vector<std::string> findPossibleTFs(sqlite3* pointerOnDb,
                                         char* messageError) {
    std::string sql = "SELECT DISTINCT EXPIREPERIOD FROM RESULTSSTAVKI;";
    std::vector<std::string> vectorWithTFs;
    int exit = sqlite3_exec(pointerOnDb, sql.c_str(),
                            callbackForFindingTFValues, &vectorWithTFs, &messageError);
    if (exit != SQLITE_OK) {
        std::cout << "Error trying to exec sql " << sql << ". messageError - "
                  << messageError << ", exit = " << exit << std::endl;
        std::exit(-1);
    }
    return vectorWithTFs;
}