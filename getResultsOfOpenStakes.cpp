#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#include "getResultsOfOpenStakes.h"
#include "json.hpp"
#include "sqlite3.h"
#include "constant.h"
// Из huntForSignals мне нужна ф-я findNewRandomNumber - с её помощью нахожу новый id для нового
// строчки в resultsDb (ф-я подбирает новый не используемый id)
#include "huntForSignals.h"

void createResultsDbAndTableIfDoesntExist(std::string tableName) {
    sqlite3* resultsDb;
    int exit = 0;
    exit = sqlite3_open(resultsDbPath.c_str(), &resultsDb);

    if (exit) {
        std::cerr << "Error opening results db - " << sqlite3_errmsg(resultsDb) << std::endl;
        std::exit(-1);
    } else {
        std::cout << "Opened results Database successfully!" << std::endl;
    }

    std::string sql = "CREATE TABLE IF NOT EXISTS " + tableName +"("
                      "ID               INT     PRIMARY KEY   NOT NULL, "
                      "PAIRNAME         TEXT    NOT NULL, "
                      "DATEWHENSET      TEXT    NOT NULL, "
                      "VALUEWHENSET     TEXT    NOT NULL, "
                      "DATEWHENEXPIRED  TEXT    NOT NULL,"
                      "VALUEWHENEXPIRED TEXT    NOT NULL, "
                      "EXPIREPERIOD     TEXT    NOT NULL, "
                      "CERTAIN          INT     NOT NULL, "
                      "CHECKTIME        TEXT    NOT NULL,"
                      "SUCCESS          TEXT    NOT NULL );";

    char* messageError;
    exit = sqlite3_exec(resultsDb, sql.c_str(), NULL, NULL, &messageError);

    if (exit != SQLITE_OK) {
        std::cout << "Error while creating results db table. messageError - " <<
        messageError << std::endl;
        std::exit(-1);
    }
//    else {
//        std::cout << "Successfully created table in results db" << std::endl;
//    }

    sqlite3_close(resultsDb);
}

/* TODO:
 * 1. В ф-и writeInResultsDb должно быть CREATE TABLE IF DOESNT EXIST
 * ID в results не обязано быть таким же, как в current, но надо быть точно уверенным, что
 * после вставки
 * в results строка исчезнет из current, и никак не получится снова её вставить в results
 * не помню, что ещё
 */

void deleteFromCurrentDb(std::string idForDelete) {

    sqlite3* currentDb;
    char* messageError;
    int exit = sqlite3_open(currentDbPath.c_str(), &currentDb);

    if (exit) {
        std::cerr << "Error opening current db - " << sqlite3_errmsg(currentDb) << std::endl;
        std::exit(-1);
    } else {
//        std::cout << "Opened current database successfully!" << std::endl;
    }

    std::string sql = "DELETE FROM " + g_currentDbTableName + " WHERE ID='" + idForDelete + "';";

    exit = sqlite3_exec(currentDb, sql.c_str(), NULL, NULL, &messageError);

    if (exit != SQLITE_OK) {
        std::cout << "Error while deleting from current db. messageError - " <<
        messageError << std::endl;
        std::exit(-1);
    } else {
        std::cout << "Succesfully deleted row from current db (gonna "
                     "add this row to results db)" << std::endl;
    }

    sqlite3_close(currentDb);

}



void writeInResultsDb(bool success, char** stackToWriteInResult, std::string currentPrice) {
    //       0 1       2                   3      4                   5   6  7
    // I get 2 EUR/USD 2020-08-17 10:45:00 1.1849 2020-08-17 12:45:00 2h  2  15m

    std::string resultsTableName = g_resultsDbTableName;

//    createResultsDbAndTableIfDoesntExist(resultsTableName);

    sqlite3* resultsDb;
    char* messageError;

    int exit = sqlite3_open(resultsDbPath.c_str(), &resultsDb);

    if (exit) {
        std::cerr << "Error opening results db - " << sqlite3_errmsg(resultsDb) << std::endl;
        std::exit(-1);
    }
//    else {
//        std::cout << "Opened results database successfuly!" << std::endl;
//    }

    // Создаю переменную sql и добавляю туда ID
    // Предварительно определив id для нового row
    // currentRoundedPrice используется в качестве seed (ещё используется текущее время)
    int currentRoundedPrice = round(std::stod(currentPrice) * 1000);
    int generatedNewId = findNewRandomNumber(resultsDb,
                                             std::to_string(currentRoundedPrice),
                                             resultsTableName,
                                             1000000);
    std::string sql = "INSERT INTO " + resultsTableName + " VALUES(" + std::to_string(generatedNewId);
    // Здесь i начинается с 1, так как строчкой раньше я вручную добавляю stackToWriteInResult[0]
    // Это можно изменить, но тогда надо и в цикле ниже из sql убрать ',', которая ставится всегда

    sql += ", '";
    // Добавляю PAIRNAME
    sql += stackToWriteInResult[1];
    sql += "', '";
    // Добавляю DATEWHENSET
    sql += stackToWriteInResult[2];
    sql += "', '";
    // Добавляю VALUEWHENSET
    sql += stackToWriteInResult[3];
    sql += "', '";
    // Добавляю DATEWHENEXPIRED
    sql += stackToWriteInResult[4];
    sql += "', '";
    // Добавляю VALUEWHENEXPIRED (она хранится не в stackToWriteInResult.
    // В упомянутой переменной хранятся значения из currentBd, где открытые ставки
    // А текущую цену (VALUEWHENEXPIRED) я беру из другого места (только что скачанного файла))
    sql += currentPrice;
    sql += "', '";
    // Добавляю EXPIREPERIOD
    sql += stackToWriteInResult[5];
    sql += "', '";

    // в stackToWriteInResult[6] хранится UP, показатель, куда
    // предположительно пойдёт цена (вверх или вниз)

    // Добавляю CERTAIN
    sql += stackToWriteInResult[7];
    sql += "', '";
    // Добавляю CHECKTIME
    sql += stackToWriteInResult[8];

    // Добавляю SUCCESS
    sql += "', '" + std::to_string(success) + "');";

//    std::cout << "sql:" << std::endl << sql << std::endl;

    exit = sqlite3_exec(resultsDb, sql.c_str(), NULL, NULL, &messageError);

    if (exit) {
        std::cout << "Couldn't INSERT row in results db. messageError - " << messageError <<
        std::endl;
        std::exit(-1);
    } else {
//        std::cout << "Successful INSERT of a new row in results db" << std::endl;
    }

    sqlite3_close(resultsDb);
//                      "\"INSERT INTO " + g_currentDbTableName + " VALUES(1, 'EUR/USD',
//                      '2020-08-17 10:45:00', '1.1849',
//                      '2020-08-17 10:46:00', '1m', 0, '5m');\""
}

// Вот здесь тонкий момент. Если сайт *** решат изменить номера, под которыми идут пары - программа
// будет выкидывать исключение.
std::string returnCurrencyNumber(std::string currencyName) {
    if (currencyName == "EUR/USD") {
        return "1";
    }
    if (currencyName == "GBP/USD") {
        return "2";
    }
    if (currencyName == "USD/JPY") {
        return "3";
    }
    if (currencyName == "AUD/USD") {
        return "5";
    }
    if (currencyName == "USD/CAD") {
        return "7";
    }
    if (currencyName == "EUR/JPY") {
        return "9";
    }
    if (currencyName == "EUR/CHF") {
        return "10";
    }

    std::cerr << "getResultsOfOpenStakes.cpp - returnCurrencyNumber function - got unexpected value,"
                 "it is " << currencyName << std::endl;
    std::cerr << "Exiting now" << std::endl;
    std::exit(-1);
}

static int callbackForWritingDownResults(void* data, int argc, char** argv, char** azColName) {

    // Needed row is stored in argv

//    for (int i = 0; i < argc; i++) {
//        std::cout << i << " = " << argv[i] << std::endl;
//    }

    std::string priceWhenSet = argv[4];

    nlohmann::json jsonObject;

    // Для записи рез-тов мне нужна только текущая цена (для сравнения с ценой при открытии
    // ставки), поэтому я могу просто посмотреть цены в файле
    // jsonDataFor5mins.txt, так как этот файл я скачиваю при каждом запуске
    // программы, и там есть текущие актуальные цены
    std::string completeJsonFileName = g_jsonFilename + "For5mins.txt";
    std::fstream fstreamOfJsonFile(completeJsonFileName.c_str());
    jsonObject = nlohmann::json::parse(fstreamOfJsonFile);

    std::string numberInJsonObject = returnCurrencyNumber(argv[1]);

//    std::cout << "numberInJsonObject = " << numberInJsonObject << std::endl;
//    std::cout << "jsonObject = " << jsonObject << std::endl;
    std::string currentPrice = jsonObject[numberInJsonObject]["summaryLast"];

//    std::cout << "The id of open stake is " << argv[0] << std::endl;
//    std::cout << "Currency name is " << argv[1] << std::endl;
//    std::cout << "currentPrice = " << currentPrice << std::endl;
//    std::cout << "priseWhenSet = " << argv[3] << std::endl;

    if (currentPrice > argv[3]) {
        writeInResultsDb(true, argv, currentPrice);
        // TODO: вот этот момент - здесь пытаюсь удалить запись, будучи в callback.
        //  Но этот когда этот
        // callback вызывается, currentDb уже открыта. 99%, что из-за этого получаю ошибку
        // deleteFromCurrentDb(argv[0]);

    } else {
        writeInResultsDb(false, argv, currentPrice);
//        deleteFromCurrentDb(argv[0]);
    }

    return 0;
}

void getResultsOfOpenStakes(const char* pathToCurrentDb,
                            const char* pathToResultsDb,
                            std::vector<std::string> vectorIdsOfOpenStakes,
                            std::string fileNameWithJson) {

    sqlite3* currentDB;
    char* messageError;
    int exit = sqlite3_open(pathToCurrentDb, &currentDB);

    createResultsDbAndTableIfDoesntExist(g_resultsDbTableName);

    if (exit) {
        std::cerr << "Error opening current DB " << sqlite3_errmsg(currentDB) << std::endl;
        std::exit(-1);
    }
//    else
//        std::cout << "Opened current Database successfully!" << std::endl;

    for (auto id : vectorIdsOfOpenStakes) {
        std::string sql = "SELECT * FROM " + g_currentDbTableName + " WHERE ID='" + id + "';";

        // TODO: здесь начинается callback, тут база ещё открыта. Мб она открыта и потом
        exit = sqlite3_exec(currentDB, sql.c_str(), callbackForWritingDownResults,
                            NULL, &messageError);
        if (exit != SQLITE_OK) {
            std::cerr << "Error while selecting IDs of open stakes for writing them in "
                         "results db - " << sqlite3_errmsg(currentDB) << std::endl;
            std::exit(-1);
        }
        else {
//            std::cout << "getResultsOfOpenStakes - Successfully selected got IDs of "
//                         "open stakes for "
//                         "writing them in results db!" << std::endl;
        }

        sql = "DELETE FROM " + g_currentDbTableName + " WHERE ID='" + id + "';";
        exit = sqlite3_exec(currentDB, sql.c_str(), NULL,
                            NULL, &messageError);
        if (exit != SQLITE_OK) {
            std::cerr << "Error while deleting rows with IDs of open "
                         "stakes AFTER writing them in "
                         "results db - " << sqlite3_errmsg(currentDB) << std::endl;
            std::exit(-1);
        }
        else {
//            std::cout << "getResultsOfOpenStakes - Successfully deleted rows with IDs "
//                         "of open stakes AFTER "
//                         "writing them in results db!" << std::endl;
        }

    }
}
