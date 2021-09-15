//
// Created by c on 15.08.2020.
//

#ifndef WINRATE_CONSTANT_H
#define WINRATE_CONSTANT_H

#include <string>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <iostream>

inline std::string findCurrentTime(std::string format) {
    time_t now = time(0);
    // gmtime(&now) returns a pointer to tm struct in UTC
    tm *gmtm = gmtime(&now);
    // Я буду возвращать string, а не tm, потому что для моих задач (1. Записывать SQLite, где можно
    // хранить только TEXT (нет типа DATE); 2. Сравнивать между собой. Если хранить даты в формате
    // YY-MM-DD часы:минуты, то можно сравнивать между собой string в этом формате
    std::cout << "constant.h put_time result = " << std::put_time(gmtm, format.c_str()) << std::endl;

    std::stringstream sstream;
    sstream << std::put_time(gmtm, format.c_str());
    return sstream.str();
}
inline const std::string g_format = "%Y-%m-%d %H:%M";
inline const std::string g_currentTime = findCurrentTime(g_format);
inline const std::string g_resultsDbTableName = "RESULTSSTAVKI";
inline const std::string g_currentDbTableName = "OPENSTAVKI";
inline const std::string currentDbPath = "dbDir/current.db";
inline const std::string resultsDbPath = "dbDir/results.db";
inline const std::string g_jsonFilename = "jsonData";

struct OpenStakeStruct {
    int ID;
    std::string PAIRNAME;
    std::string VALUEWHENSET;
    std::string DATEWHENEXPIRES;
    std::string EXPIREPERIOD;
    int CERTAIN;
    std::string CHECKTIME;
};

#endif //WINRATE_CONSTANT_H

    // Another way to compare dates

//    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
//    time_t laterNow = time(0);
//    tm *laterGmtm = gmtime(&laterNow);
//    std::string laterGmtmInString = asctime(laterGmtm);
//
//    if (stringForReturn > laterGmtmInString) {
//        std::cout << "gmtmInString > laterGmtmInString\n";
//        std::cout << "gmtmInString = " << stringForReturn;
//        std::cout << "laterGmtmInString = " << laterGmtmInString << std::endl;
//    } else {
//        std::cout << "gmtmInString < laterGmtmInString\n";
//        std::cout << "gmtmInString = " << stringForReturn;
//        std::cout << "laterGmtmInString = " << laterGmtmInString << std::endl;
//    }