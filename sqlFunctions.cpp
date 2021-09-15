//
// Created by c on 12.09.2021.
//

#include <iostream>

#include "sqlite3.h"
#include "sqlFunctions.h"
#include "constants.h"

bool toRevert = false;

int callbackWriteDataToList(void* context,  // user-provided object (4th param to sqlite3_exec)
                            int columnCount,      // number of columns
                            char** columnValues,  // array of column values as C-style strings
                            char** columnName) {
//    Плохой код, но работает. Пока мне больше и не нужно
    stakeFromDB newStakeFromCallback;
    newStakeFromCallback.id = stoi(columnValues[0]);
    newStakeFromCallback.pairname = columnValues[1];
    newStakeFromCallback.dateWhenSet = columnValues[2];
    newStakeFromCallback.valueWhenSet = stof(columnValues[3]);
    newStakeFromCallback.dateWhenExpired = columnValues[4];
    newStakeFromCallback.valueWhenExpired = stof(columnValues[5]);
    newStakeFromCallback.expirePeriod = columnValues[6];
    newStakeFromCallback.certain = stoi(columnValues[7]);
    newStakeFromCallback.checkTime = columnValues[8];
    newStakeFromCallback.success = stoi(columnValues[9]);

    if (toRevert) {
//        cout << "Переворачиваю success!" << endl;
        newStakeFromCallback.success = not newStakeFromCallback.success;
    }

//    cout << "newStakeFromCallback = " << newStakeFromCallback << endl;

    list<stakeFromDB>* listOfStakes = static_cast<list<stakeFromDB>*>(context);
    listOfStakes->push_back(newStakeFromCallback);

    return 0;
}

void writeDataToList(list<stakeFromDB> &listOfStakesFromDB,
                     string pairname,
                     string timeframe,
//                     certain and roundTime at least WERE defined in header file to this cpp file
//                     so here they go without definitions, but they should be because
//                     these arguments are optional
                     int certain,
                     int roundTime,
                     bool& revert) {

//    Reversing the shirt and using toRevert variable which should be accessible from both this
//    function and callback
    revert = not revert;
    toRevert = revert;

//    if (toRevert) {
//        cout << "Буду переворачивать success!" << endl;
//    }

    sqlite3* DB;
    char* messageError;
    int exit = 0;
    exit = sqlite3_open(pathToResultsDb, &DB);

    if (exit != SQLITE_OK) {
        std::cerr << "Error opening DB " << sqlite3_errmsg(DB) << std::endl;
        std::exit(-1);
    }


    string sql = "select * from RESULTSSTAVKI WHERE PAIRNAME=\"" + pairname + "\" AND EXPIREPERIOD=\"" + timeframe + "\"";
    if (certain != -1) {
        sql += " AND CERTAIN=" + to_string(certain);
    }
    if (roundTime != -1) {
//        So I get something like 'substr(DATEWHENSET, 16, 1) in('0', '5');'
//        So I take stakes with only *0 and *5, so every 5 minutes
//                      or with only *1 and *5, so every 5 minutes counting from 1
        sql += " AND substr(DATEWHENSET, 16, 1) in('" +
                to_string(0 + certain) + "', '" +
                to_string(5 + certain) + "');";
    }
    sql += " ORDER BY DATEWHENSET";
    sql += ";";

    cout << "sql:\n" << sql << endl;
//    abort();

    exit = sqlite3_exec(DB, sql.c_str(), callbackWriteDataToList, &listOfStakesFromDB, &messageError);

    if (exit != SQLITE_OK) {
        std::cerr << "Error getting data from table - " << messageError << std::endl;
        sqlite3_free(messageError);
    }
    else
        std::cout << "Got data from table successfully" << std::endl;
}