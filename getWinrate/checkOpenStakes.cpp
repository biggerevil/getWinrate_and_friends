#include <string>
#include <iostream>
#include <vector>

#include "checkOpenStakes.h"
#include "sqlite3.h"
#include "constant.h"

// TODO: здесь много кода работы с sqlite повторяется. Чтобы это был хороший код (хорошо организованный?), можно
// завести отдельный .cpp файл (вместе с .h), где определить все основные функции, чтобы можно было писать просто
// openDatabase(pointerOnDB), runSql(pointerOnDB, sqlCommand), closeDatabase(pointerOnDB)

// Callback function is called ONCE per each row
// argc - это кол-во столбцов
static int callbackFindOpenStakesWithVector(void* data, int argc, char** argv, char** azColName) {
    // Четвёртый параметр sqlite3_exec в callback идёт первым параметром, то есть
    // В data имеем указатель, интерпретируем его как std::vector<std::string>

    std::vector<std::string>* idsForOpenStakesVector = reinterpret_cast<std::vector<std::string>*>(data);
    if (argv[4] <= g_currentTime) {

        idsForOpenStakesVector->push_back(argv[0]);
    }
    return 0;
}

void fillDbWhileDevelopingProgram(const char* currentDbPath) {

    sqlite3* DB;
    char* messageError;
    int exit = 0;
    exit = sqlite3_open(currentDbPath, &DB);

    if (exit != SQLITE_OK) {
        std::cerr << "Error opening DB " << sqlite3_errmsg(DB) << std::endl;
        std::exit(-1);
    }
//    else
//        std::cout << "Opened Database Successfully!" << std::endl;

    std::string sql = "";
    sql = "DROP TABLE " + g_currentDbTableName;
    exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messageError);

    if (exit != SQLITE_OK) {
        std::cerr << "Error deleting Table - " << messageError << std::endl;
        sqlite3_free(messageError);
    }
    else
        std::cout << "Table deleted Successfully" << std::endl;

    sql = "CREATE TABLE " + g_currentDbTableName + "("
                      "ID               INT     PRIMARY KEY   NOT NULL, "
                      "PAIRNAME         TEXT    NOT NULL, "
                      "DATEWHENSET      TEXT    NOT NULL, "
                      "VALUEWHENSET     TEXT    NOT NULL, "
                      "DATEWHENEXPIRES  TEXT    NOT NULL, "
                      "EXPIREPERIOD     TEXT    NOT NULL,"
                      "UP               TEXT    NOT NULL, "
                      "CERTAIN          TEXT    NOT NULL, "
                      "CHECKTIME        TEXT    NOT NULL );";

    exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messageError);

    if (exit != SQLITE_OK) {
        std::cerr << "Error Create Table - " << messageError << std::endl;
        sqlite3_free(messageError);
    }
    else
        std::cout << "Table created Successfully" << std::endl;

    sql =      "INSERT INTO " + g_currentDbTableName + " VALUES(1, 'EUR/USD', '2020-08-22 10:45', '1.1849', '2020-08-23 10:46', '1m', 'true', '0', '5m');"
               "INSERT INTO " + g_currentDbTableName + " VALUES(2, 'EUR/USD', '2020-08-22 10:45', '1.1849', '2020-08-23 12:45', '2h', 'true', '1', '5m');"
               "INSERT INTO " + g_currentDbTableName + " VALUES(3, 'USD/JPY', '2020-08-22 10:45', '106.35', '2020-08-23 17:45', '7h', 'true', '2', '30m');";

    exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messageError);
    if (exit != SQLITE_OK) {
        std::cerr << "Error Insert - " << messageError << std::endl;
        sqlite3_free(messageError);
    }
    else
        std::cout << "Records created Successfully!" << std::endl;

    sqlite3_close(DB);

}

void createTableIfDoesntExist(sqlite3* DB) {
    std::string sql = "";
    char* messageError;
    sql = "CREATE TABLE IF NOT EXISTS " + g_currentDbTableName + "("
                                                                 "ID               INT     PRIMARY KEY   NOT NULL, "
                                                                 "PAIRNAME         TEXT    NOT NULL, "
                                                                 "DATEWHENSET      TEXT    NOT NULL, "
                                                                 "VALUEWHENSET     TEXT    NOT NULL, "
                                                                 "DATEWHENEXPIRES  TEXT    NOT NULL, "
                                                                 "EXPIREPERIOD     TEXT    NOT NULL,"
                                                                 "UP               TEXT    NOT NULL, "
                                                                 "CERTAIN          TEXT    NOT NULL, "
                                                                 "CHECKTIME        TEXT    NOT NULL );";

    int exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messageError);

    if (exit != SQLITE_OK) {
        std::cerr << "Error Create Table - " << messageError << std::endl;
        sqlite3_free(messageError);
    }
    else
        std::cout << "Table check for existence and maybe creation went successfully" << std::endl;
}

void findOpenStakes(const char* currentDbPath, std::string currentTime, std::vector<std::string>& vectorForStoringIds) {

    sqlite3* DB;
    char* messageError;
    int exit = sqlite3_open(currentDbPath, &DB);

    if (exit) {
        std::cerr << "Error open DB " << sqlite3_errmsg(DB) << std::endl;
        std::exit(-1);
    }
//    else
//        std::cout << "Opened current Database Successfully!" << std::endl;

    createTableIfDoesntExist(DB);

    std::string sql = "SELECT * FROM " + g_currentDbTableName + ";";

    exit = sqlite3_exec(DB, sql.c_str(), callbackFindOpenStakesWithVector, &vectorForStoringIds, &messageError);
    if (exit != SQLITE_OK) {
        std::cerr << "Error in SELECT * from DB - " << sqlite3_errmsg(DB) << std::endl;
        std::exit(-1);
    }
//    else {
//        std::cout << "Successfully selected * while looking for open stakes!" << std::endl;
//    }
}

std::vector<std::string> checkOpenStakes(const char* dbDir, const char* currentTime) {
    std::string pathForCurrent = std::string(dbDir) + "/current.db";

//    fillDbWhileDevelopingProgram(pathForCurrent.c_str());
    std::vector<std::string> vectorForStoringIds;
    findOpenStakes(pathForCurrent.c_str(), currentTime, vectorForStoringIds);

    if (vectorForStoringIds.empty()) {
        std::cout << "No expired open stakes were found" << std::endl;
    } else {
        std::cout << "Found expired stakes!" << std::endl;
//        std::cout << "checkOpenStakes.cpp - So this are ids which EXPIRETIME is now (or was):" << std::endl;
//        for (auto id : vectorForStoringIds) {
//            std::cout << "id = " << id << std::endl;
//        }
    }

    return vectorForStoringIds;
}