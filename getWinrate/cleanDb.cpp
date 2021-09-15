#include "cleanDb.h"
#include "sqlite3.h"
#include "constant.h"

#include <iostream>
#include <string>
#include <vector>

/*
 * Принцип работы:
 *
 * 1. SELECT *
 * 2. В callback по очереди поступают все эти столбцы
 * 3. В callback смотрю дату. Если там неровные минуты, то удаляю
 * 4. Удалять можно двумя способами:
 *    1. Сразу. Не уверен, что так нормально сработает. Как-то опасным кажется, не знаю
 *    2. Сохранять список id в вектор (или другую структуру данных, полегче, типа stack'а или queue например) и
 *      затем типа итерироваться по этой структуре (мб не совсем итерироваться) и удалять все id... Или,
 *      если можно, передать все id в одном sql-запросе
 * 5. Ещё можно запоминать дату последней очистки в другом файле (типа lastCleanDate.txt) и
 *    при следующих запусках select'ом брать только значения позже этой даты (включительно), и,
 *    соответственно, обновлять дату в файле
 */

std::vector<int> idsForDelete{};

static int callbackForWritingDownResults(void* data, int argc, char** argv, char** azColName) {

    // int argc: holds the number of results
    // (array) azColName: holds each column returned
    // (array) argv: holds each value

    std::string dateLine = azColName[2];
    int min = std::stoi(dateLine.substr(14,2));
    if (min % 5 != 0) {
        idsForDelete.push_back(std::stoi(argv[0]));
    }

    // Return successful
    return 0;
}

int cleanDb() {

    sqlite3* currentDb;
    char* messageError;
    int exit = sqlite3_open(currentDbPath.c_str(), &currentDb);

    if (exit) {
        std::cerr << "Error opening current db - " << sqlite3_errmsg(currentDb) << std::endl;
        std::exit(-1);
    }

    std::string sql = "SELECT * FROM " + g_currentDbTableName;

    // if есть файл {if есть дата, то sql += WHERE DATEWHENSET >= "дата"}

    exit = sqlite3_exec(currentDb, sql.c_str(), NULL, NULL, &messageError);

    if (exit != SQLITE_OK) {
        std::cout << "Error while deleting from current db. messageError - " <<
                  messageError << std::endl;
        std::exit(-1);
    } else {
        std::cout << "Successfully deleted row from current db (gonna "
                     "add this row to results db)" << std::endl;
    }

    std::cout << "idsForDelete:" << std::endl;

    for (int id : idsForDelete) {
        std::cout << id << std::endl;
    }
    std::cout << "\nEnd of idsForDelete.\n" << std::endl;

    // if есть файл, то обнулить его, если файла нет, то создать
    // открываем файл
    // записываем дату
    // закрываем файл

    sqlite3_close(currentDb);
    return 0;

}