#include "huntForSignals.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <cstdlib>
#include <ctime>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <cstdlib>

#include "json.hpp"
#include "sqlite3.h"
#include "constant.h"
#include "beastWebSocket.h"
#include "pythonBinaryApi.h"
#include "Python.h"

inline const std::string wordForUp = "true";
inline const std::string wordForDown = "false";
inline const std::string wordForNothing = "no";

extern bool shouldIRunRubyCode;
extern bool willGoHigher;

extern bool USDJPY_10h_shouldMakeStake;
extern bool USDJPY_10h_willGoHigher;
extern bool USDJPY_7h_shouldMakeStake;
extern bool USDJPY_7h_willGoHigher;
extern bool EURJPY_1h_shouldMakeStake;
extern bool EURJPY_1h_willGoHigher;
extern bool USDCAD_2h_shouldMakeStake;
extern bool USDCAD_2h_willGoHigher;

// 14 september, 2021
extern bool EURJPY_15m_shouldMakeStake;
extern bool EURJPY_15m_willGoHigher;

static int callbackForCheckingDuplicate(void* data, int argc, char** argv, char** azColName) {

    bool* thereIsDuplicate = reinterpret_cast<bool*>(data);
    // Меняем значение, так как сам callback вызывается только в тех случаях, когда дубликат есть
    *thereIsDuplicate = true;

//    std::cout << "Here what have with selected ID" << std::endl;
//    for (int i = 0; i < argc; i++) {
//        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
//    }

    return 0;
}

std::string findExpireDate(std::string currentTime, int minutesBeforeExpire) {
    // Я храню формат в глобальной переменной (в файле constant.h), чтобы при его изменении всё
    // не поломалось
    std::string format = g_format;
    // Превращаем текущую дату в time_t, чтобы затем прибавить кол-во секунд и превратить обратно
    // в string. Таким образом получаем дату экспирации
    // Сначала превращаем в tm
    struct tm currentTm = {};
    std::stringstream ss(g_currentTime);
    ss >> std::get_time(&currentTm, g_format.c_str());

    // Затем превращаем в time_t
    time_t currentTimeT = mktime(&currentTm);

    // Добавляем необходимое кол-во секунд
    currentTimeT += minutesBeforeExpire * 60;

    struct tm* expireGmtm = localtime(&currentTimeT);
    ss.str("");
    ss.clear();
    ss << std::put_time(expireGmtm, format.c_str());
    return ss.str();
}

std::string findPeriod(std::string currentDate, std::string expireDate) {
    std::string format = g_format;
    struct tm currentTm = {};
    std::stringstream ssForCurrent(currentDate);
    ssForCurrent >> std::get_time(&currentTm, g_format.c_str());

    struct tm expireTm = {};
    std::stringstream ssForExpire(expireDate);
    ssForExpire >> std::get_time(&expireTm, g_format.c_str());

    time_t currentTimeT = mktime(&currentTm);
    time_t expireTimeT = mktime(&expireTm);

    time_t periodTimeT = expireTimeT - currentTimeT;

    std::string periodForReturn = "";
    // Если periodTimeT меньше часа
    if (periodTimeT < 3600) {
        periodForReturn += std::to_string(periodTimeT / 60);
        periodForReturn += "m";
    } else
        // Если periodTimeT меньше суток
    if (periodTimeT < 86400) {
        periodForReturn += std::to_string(periodTimeT / (60 * 60));
        periodForReturn += "h";
    } else
        // Есди periodTimeT сутки или больше
    if (periodTimeT >= 86400) {
        periodForReturn += std::to_string(periodTimeT / (60 * 60 * 24));
        periodForReturn += "d";
    }

    return periodForReturn;

//    std::cout << "periodTimeT = " << periodTimeT << std::endl;
//    std::cout << "periodTimeT / 60 (mins) = " << periodTimeT / 60 << std::endl;
//    std::cout << "periodTimeT / (60 * 60) (hours) = " << periodTimeT / (60 * 60)  << std::endl;
//    std::cout << "periodTimeT / (60 * 60 * 24) (days) = " <<
//    periodTimeT / (60 * 60 * 24) << std::endl;
//    return std::to_string(periodTimeT);
}


// TODO: тут дублирование кода, было бы неплохо исправить это, но я пока могу подумать только о
//  том, чтобы придумать ещё одну функцию count(firstValue, secondValue), и в зависимости от
//  toDown свапать value местами
int countTiIndicators(std::string tiBuy, std::string tiSell, std::string toDown) {

    if (toDown == wordForDown) {
        if (tiSell == "11") {
            // Самый сильный сигнал
            return 1;
        } else if (tiSell == "10") {
            if (tiBuy == "0") {
                // Почти самый сильный сигнал
                return 2;
            }
            // 3 по силе сигнал (10 инд. показывают Buy, 1 инд. показывает Sell)
            return 3;

        } else if (tiSell == "9") {
            if (tiBuy == "0") {
                return 4;
            } else if (tiBuy == "1") {
                return 5;
            }
            // Если дошли до сюда, то tiBuy = "2"
            return 6;

        } else if (tiSell == "8") {
            if (tiBuy == "0") {
                return 7;
            } else if (tiBuy == "1") {
                return 8;
            } else if (tiBuy == "2") {
                return 9;
            } else if (tiBuy == "3") {
                return 10;
            }
            // Если дошли до сюда, то tiBuy = "4"
            return 11;
        }
    } else {

        if (toDown == wordForDown) {
            if (tiBuy == "11") {
                // Самый сильный сигнал
                return 1;
            } else if (tiBuy == "10") {
                if (tiSell == "0") {
                    // Почти самый сильный сигнал
                    return 2;
                }
                // 3 по силе сигнал (10 инд. показывают Buy, 1 инд. показывает Sell)
                return 3;

            } else if (tiBuy == "9") {
                if (tiSell == "0") {
                    return 4;
                } else if (tiSell == "1") {
                    return 5;
                }
                // Если дошли до сюда, то tiBuy = "2"
                return 6;

            } else if (tiBuy == "8") {
                if (tiSell == "0") {
                    return 7;
                } else if (tiSell == "1") {
                    return 8;
                } else if (tiSell == "2") {
                    return 9;
                } else if (tiSell == "3") {
                    return 10;
                }
                // Если дошли до сюда, то tiBuy = "4"
                return 11;
            }
        }
    }
    return 100;
}

std::pair<int, std::string> findCertain(std::map<std::string, std::string>& joinedIndicatorsData) {
    std::pair<int, std::string> pairToReturn;
    if (joinedIndicatorsData["maBuy"] == "0") {
        pairToReturn.second = wordForDown;
        // TODO: Походу на самом деле технических индикаторов 11
        //  чтобы убедиться в том, что их 11, зайди на сайт ***, сделай поиск по
        //  странице "технические индикаторы", и там внизу будет Покупать: * Продавать: *
        //  Нейтрально: *
        pairToReturn.first = countTiIndicators(joinedIndicatorsData["tiBuy"],
                                               joinedIndicatorsData["tiSell"],
                                               wordForDown);
        return pairToReturn;

    } else if (joinedIndicatorsData["maSell"] == "0") {
        pairToReturn.second = wordForUp;

        pairToReturn.first = countTiIndicators(joinedIndicatorsData["tiBuy"],
                                               joinedIndicatorsData["tiSell"],
                                               wordForUp);
        return pairToReturn;
    }

    // В ф-и findSignalInFile в качестве успешного нахождения сигнала стоит проверка if (
    // certain < 4) (на данный момент < 4, возможно, потом будет значения несколько больше),
    // поэтому, чтобы точно знать, что сигнал не был найден, я передаю 100 (так как 100 куда
    // больше 4)
    pairToReturn.second = wordForNothing;
    pairToReturn.first = 100;
    return pairToReturn;
}

int findNewRandomNumber(sqlite3* currentDb, std::string summaryLast, std::string tableName,
                        int upperBound) {
    // TODO: Нужно добавить row в current.db. Как определить id будущего row? SELECT'ом выбирать все значения,
    // определять последнее? В принципе даже ведь не важно, чтобы id шли по порядку, надо, чтобы просто
    // какие-нибудь были. Я на основе ID никаких выводов не делаю, я с его помощью только
    // добавляю-удаляю строчки
    //
    // Можно рандомно брать одно число от 10 до 100 и использовать в качестве id его. Если ни одно такое
    // значение не подходит, брать рандомно от 101 до 10000

    // Определяет seed с помощью комбинации системного времени и текущей цены записываемой ставки

    // Брать текущую цену из joinedIndicatorsData
    // текущую цену можно передавать в joinedIndicatorsData
    int currentRoundedPrice = stoi(summaryLast);
    // TODO: Здесь можно добавить ещё миллисекунды
    // https://stackoverflow.com/questions/19555121/how-
    // to-get-current-timestamp-in-milliseconds-since-1970-just-the-way-java-gets
//    srand(static_cast<unsigned int>(time(0)) + currentRoundedPrice);
    srand(static_cast<unsigned int>(currentRoundedPrice + time(0)));

    static const double fraction = 1.0 / (static_cast<double>(RAND_MAX) + 1.0);
    int min = 10;
    int max = upperBound;
    int generatedRandomNumber = static_cast<int>(rand() * fraction * (max - min + 1) + min);

    std::string sql = "SELECT * FROM " + tableName + " WHERE ID='" +
                      std::to_string(generatedRandomNumber) + "';";
    bool thereIsDuplicate = false;

    char* messageError;

    int exit = sqlite3_exec(currentDb, sql.c_str(), callbackForCheckingDuplicate,
                            &thereIsDuplicate, &messageError);
    if (exit != SQLITE_OK) {
        std::cout << "Error trying to select when hunting for signal. messageError - "
                  << messageError << ", exit = " << exit << std::endl;
        std::exit(-1);
    } else {
//        std::cout << "Success selecting while hunting for signals!" << std::endl;
    }

    // Ищем новый подходящий ID, пока не найдём значение без дубликатов
    while (thereIsDuplicate == true) {

        generatedRandomNumber = static_cast<int>(rand() * fraction * (max - min + 1) + min);
        thereIsDuplicate = false;
        sql = "SELECT * FROM " + tableName + " WHERE ID='" + std::to_string(generatedRandomNumber) + "';";
        exit = sqlite3_exec(currentDb, sql.c_str(), callbackForCheckingDuplicate,
                            &thereIsDuplicate, &messageError);
        if (exit != SQLITE_OK) {
            std::cout << "Error trying to select when hunting for signal. messageError - "
                      << messageError << ", exit = " << exit << std::endl;
            std::exit(-1);
        } else {}

    }

    return generatedRandomNumber;
}

void addToCurrentDb(std::map<std::string,
        std::string>& joinedIndicatorsData,
                    int certainValue,
                    std::string direction) {

    sqlite3* currentDb;
    char* messageError;

    int exit = sqlite3_open(currentDbPath.c_str(), &currentDb);

    std::cout << "#11" << std::endl;
    if (exit) {
        std::cerr << "huntForSignals - Error opening current db - " << sqlite3_errmsg(currentDb) << std::endl;
        std::exit(-1);
    } else {
        std::cout << "huntForSignals - Opened current database successfully!" << std::endl;
    }



    std::vector<std::string> tfsVector;

    // type(joinedIndicatorsData["minutesTimeframe"]) =
    // std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char> >

    int minutesTimeframe = std::stoi(joinedIndicatorsData["minutesTimeframe"]);
    std::cout << "#12" << std::endl;
    switch (minutesTimeframe) {
        case 5:
            tfsVector.push_back("5");
            tfsVector.push_back("15");
            tfsVector.push_back("30");
            break;
        case 15:
            tfsVector.push_back("15");
            tfsVector.push_back("30");
            tfsVector.push_back("60");
            break;
        case 30:
            tfsVector.push_back("30");
            tfsVector.push_back("60");
            break;
        case 60:
            tfsVector.push_back("60");
            tfsVector.push_back("90");
            tfsVector.push_back("120");
            break;
        case 300:
            tfsVector.push_back("300");
            tfsVector.push_back("450");
            tfsVector.push_back("600");
            break;
        case 1440:
            tfsVector.push_back("1440");
            tfsVector.push_back("2160");
            tfsVector.push_back("2880");
            break;
        default:
            std::cout << "Something wrong with minutesTimeframe in huntForSignals.cpp" << std::endl;
            std::exit(-1);
    }

    std::cout << "#13" << std::endl;

    //  + std::to_string(generatedRandomNumber) +
    // "INSERT INTO " + g_currentDbTableName + " VALUES(1, 'EUR/USD',
    // '2020-08-22 10:45:00', '1.1849', '2020-08-23 10:46:00', '1m', 0, '5m');"

    // TODO: надо написать insert'ы, чтобы вставлять значения в currentDb. Это делается внутри
    // цикла ниже
    //
    // Конвертируем текущую дату (g_currentTime) в time_t, чтобы затем приплюсовать к этой дате
    // необходимые минуты (из tfsVector) и получить DATEWHENEXPIRES (столбце в currentDb)
    // Затем можно будет из time_t DATEWHENEXPIRES вычесть time_t g_currentTime и получить
    // EXPIREPERIOD (столбец в currentDb). Ну а checkTime можно получить из
    // joinedIndicatorsData["minutesTimeframe"]
    // Ссылки:
    // см. ответ с 54 лайками = https://stackoverflow.com/
    // questions/11213326/how-to-convert-a-string-variable-containing-time-to-time-t-type-in-c
    // c++ add time to time_t = https://www.google.com/search?
    // source=hp&ei=FzVFX7PmOoHQaMPVo8AB&q=c%2B%2B+add+time+to+time_t&btnK=
    // %D0%9F%D0%BE%D0%B8%D1%81%D0%BA+%D0%B2+Google

    std::string sql = "";

    for (std::string tfValue : tfsVector) {
        sql = "INSERT INTO " + g_currentDbTableName + " VALUES(";
//        sql += generatedRandomNumber;

        int generatedRandomNumber = findNewRandomNumber(
                currentDb,
                joinedIndicatorsData["priceForFindingNewRandomNumber"],
                g_currentDbTableName,
                2000);
        sql += std::to_string(generatedRandomNumber);
        sql += ", '";
        sql += joinedIndicatorsData["summaryName"];
        sql += "', '";
        sql += g_currentTime;
        sql += "', '";
        sql += joinedIndicatorsData["summaryLast"];
        sql += "', '";
        // Теперь нужно вставить DATEWHENEXPIRES. Чтобы определить дату экспирации, надо из
        // tfsVector брать значения минут и прибавлять их к текущему времени
        std::string expireDate = findExpireDate(g_currentTime, stoi(tfValue));
        sql += expireDate;
        sql += "', '";
        // Теперь добавляем период ставки (сколько времени будет жить ставка)
        std::string period = findPeriod(g_currentTime, expireDate);
        sql += period;

        // Добавляем direction - направление, куда пойдёт цена. Столбик называется UP, а
        // возможные значение - "true", "false" и "no" (чёткие значения заданы
        // в самом верху этого файла в виде константных inline переменных).
        // Если "true", то ставка на то, что цена пойдёт вверх, если
        // "false", то ставка на то, что цена пойдёт вниз. "no" я не
        // собираюсь когда-либо использовать, на всякий случай пишу
        // это в тех ставках, которые не планирую добавлять вверх
        sql += "', '";
        sql += direction;

        // Добавляем значение CERTAIN (его я нахожу в ф-и findSignalInFile)
        sql += "', '";
        // certainValue на данный момент я передаю в функцию в int, а не в string. Так что
        // надо перед записью превратить его в string
        sql += std::to_string(certainValue);

        // Теперь добавляем CHECKTIME;
        sql += "', '";
        // minutesTimeframe тоже (как и certainValue) надо превратить в string, иначе не пишется.
        // Почему - не знаю, вроде, передаю в joinedIndicatorsData, а без превращения в string
        // в переменную sql не добавляется
        sql += std::to_string(minutesTimeframe);



        sql += "');";

//        Определяем, подходит ли эта ставка для ставки (выбирал вручную, так что всё как задумано на данный момент)
//        if (period == "10h") {
//            if (joinedIndicatorsData["summaryName"] == "USD/JPY") {
//                USDJPY_10h_shouldMakeStake = true;
//                if (direction == "true") {
//                    USDJPY_10h_willGoHigher = true;
//                }
//            }
//        }
//
//        if (period == "7h") {
//            if (joinedIndicatorsData["summaryName"] == "USD/JPY") {
//                USDJPY_7h_shouldMakeStake = true;
//                if (direction == "true") {
//                    USDJPY_7h_willGoHigher = true;
//                }
//            }
//        }
//
//        if (period == "1h") {
//            if (joinedIndicatorsData["summaryName"] == "EUR/JPY") {
//                EURJPY_1h_shouldMakeStake = true;
//                if (direction == "true") {
//                    EURJPY_1h_willGoHigher = true;
//                }
//            }
//        }

        if (period == "15m") {
            if (joinedIndicatorsData["summaryName"] == "EUR/JPY") {
                // Здесь должна быть проверка на certain ещё
                EURJPY_15m_shouldMakeStake = true;
                if (direction == "true") {
                    EURJPY_15m_willGoHigher = true;
                }
            }
        }

//        if (period == "2h") {
//            if (joinedIndicatorsData["summaryName"] == "USD/CAD") {
//                USDCAD_2h_shouldMakeStake = true;
//                if (direction == "true") {
//                    USDCAD_2h_willGoHigher = true;
//                }
//            }
//        }

//        if (period == "2h") {
//            if (joinedIndicatorsData["summaryName"] == "USD/CAD") {
//                shouldIRunRubyCode = true;
//                if (direction == "true") {
//                    willGoHigher = true;
//                }
//            }
//        }

//        That's just for test that function is called
//        makeAStakeInPython("USD/JPY", period, 666, true);

        if (certainValue == 2) {
            if (joinedIndicatorsData["summaryName"] == "USD/JPY") {
                if (period == "5m") {
                    std::cout << "Gonna make a stake - USD/JPY, ...\n";

                    // BOOST.BEAST CODE
//                    makeStakeBeast();

                    // PYTHON CODE
//                    std::system("python3 makeStake.py");


//                    // Знаю, что тут плохой код. Почему-то ещё в начале проекта захотел в direction
//                    // хранить string, а не bool (и назвав тогда не direction, а stavitVverh, например -
//                    // Тогда сразу понятно, что true значит ставить вверх)
//                    bool directionForFunction;
//                    if (direction == "true") {
//                        directionForFunction = true;
//                    } else {
//                        directionForFunction = false;
//                    }
//                    // TODO: Осторожно, тут захардкожено "USD/JPY"!
//                    // Для меня сейчас достаточно важна скорость работы, так что лучше я захардкожу, чем
//                    // буду ещё раз доставать из joinedIndicatorsData
//                    makeAStakeInPython("USD/JPY", period, 666, directionForFunction);
                }
            }
        }


        exit = sqlite3_exec(currentDb, sql.c_str(), NULL, 0, &messageError);
        if (exit != SQLITE_OK) {
            std::cerr << "Error Inserting NEW REAL stake in currenDb - " <<
                      messageError << std::endl;
            std::cerr << "generatedRandomNumber = " <<
                      std::to_string(generatedRandomNumber) << std::endl;
            std:: cerr << "Tried to add " << joinedIndicatorsData["summaryName"] <<
                       " with price = " << joinedIndicatorsData["summaryLast"] << " and expireDate = " <<
                       expireDate << std::endl;
            sqlite3_free(messageError);
        }
//        else
//            std::cout << "NEW REAL insert successful!" << std::endl;
    }

    // TODO: возможно, здесь ошибка в закрытии бд. Типа она где-то там открыта, до этой функции,
    // а я её закрываю, и получается каналья
    exit = sqlite3_close(currentDb);

    if (exit != SQLITE_OK) {
        std::cout << "Error while trying to close db in addToCurrentDb. messageError - " <<
                  messageError << ", exit = " << exit << std::endl;
    }
//    else {
//        std::cout << "Success closing db from addToCurrentDb" << std::endl;
//    }
}

void findSignalInFile(std::string jsonFileName, std::string minutesTimeframe) {
    nlohmann::json jsonObject;

    std::fstream fstreamOfJsonFile(jsonFileName.c_str());
    std::cout << "Gonna parse JSON" << std::endl;
    jsonObject = nlohmann::json::parse(fstreamOfJsonFile);

    std::vector<std::string> indices{"1", "2", "3", "5", "7", "9", "10"};
    // std::vector<int> indices{1, 2, 3, 5, 7, 9, 10};

    std::cout << "Gonna launch for and iterate over indices" << std::endl;
    for (std::string index : indices) {

        std::cout << "Iterating over indice " << index << std::endl;

        // TODO: возможность для улучшения. Разбирать одним из последних

        std::map<std::string, std::string> joinedIndicatorsData;

        // TODO: возможность для улучшения. Необязательно сначала передавать в value
        // Можно просто изменить тип joinedIndicatorsData и по-нормальному передавать int
        // Не хочу сейчас много менять, хочу наконец запустить бота
        int value = jsonObject[index]["maBuy"];
        joinedIndicatorsData["maBuy"] = std::to_string(value);
        // С нижним вариантом кода вылетает ошибка
//        joinedIndicatorsData["maBuy"] = static_cast<std::string>((jsonObject[index]["maBuy"]));
//        joinedIndicatorsData["maBuy"] = jsonObject[index]["maBuy"];
        value = jsonObject[index]["maSell"];
        joinedIndicatorsData["maSell"] = std::to_string(value);
        value = jsonObject[index]["tiBuy"];
        joinedIndicatorsData["tiBuy"] = std::to_string(value);
        value = jsonObject[index]["tiSell"];
        joinedIndicatorsData["tiSell"] = std::to_string(value);
        // TODO: Здесь можно не передавать более joinedIndicatorsData["maBuy"/"tiSell"/...],
        // так как я уже вычислил значение certain
        std::pair<int, std::string> pairWithCertainAndDirection;
        pairWithCertainAndDirection = findCertain(joinedIndicatorsData);
        int certain = pairWithCertainAndDirection.first;
        // Если не умножать на 1000, то round будет округялть до 1 или 2 (потому что
        // многие значения там 1.67584). И тогда числа получается не очень случайными
        // (я использую currentRoundedPrice для задачи в srand).
        // Чтобы это исправить, можно умножать на 1000. И тогда будет передаваться
        // 1675, что уже гораздо лучше для рандома

        if (certain < 12) {
            std::string currentPrice = jsonObject[index]["summaryLast"];
            int currentRoundedPrice = round(std::stod(currentPrice) * 1000);
            joinedIndicatorsData["priceForFindingNewRandomNumber"] = std::to_string(currentRoundedPrice);
            joinedIndicatorsData["summaryLast"] = jsonObject[index]["summaryLast"];


            joinedIndicatorsData["summaryName"] = jsonObject[index]["summaryName"];
            joinedIndicatorsData["minutesTimeframe"] = minutesTimeframe;

            std::cout << "#10" << std::endl;
            addToCurrentDb(joinedIndicatorsData, certain, pairWithCertainAndDirection.second);
        } else {
            std::cout << "Didn't find signal in this iteration" << std::endl;
        }

    }
}

void initializeSeed(std::string jsonFileName) {
    nlohmann::json jsonObject;
    std::fstream fstreamOfJsonFile(jsonFileName.c_str());
    jsonObject = nlohmann::json::parse(fstreamOfJsonFile);

    // TODO: здесь опасный момент -
    std::string currentPrice = jsonObject["1"]["summaryLast"];
//    srand(static_cast<unsigned int>(time(0)) + currentRoundedPrice);
}

void lookForSignals (std::string pathToFileWithLinks, std::string jsonFileName) {

//    Py_Initialize();

//    makeStakeBeast();

    std::fstream fstrToLinks(pathToFileWithLinks);
    std::vector<std::string> filePathsToJsonData;
    std::string link = "";
    while (fstrToLinks >> link) {
        auto equalOperatorPosition = std::find(link.end() - 8,
                                               link.end(), '=');
        std::string TFinSeconds = std::string(equalOperatorPosition + 1, link.end());
        // А затем я превращаю TFinSeconds в int, делю на 60, чтобы получить минуты,
        // снова превращаю в string, чтобы сложить с For и вставить в имя файла
        std::string forTFinMinutes = "For" + std::to_string(std::stoi(TFinSeconds) / 60);

        std::cout << "Generated path to json file with TF in minutes = " << forTFinMinutes << std::endl;
        filePathsToJsonData.push_back(jsonFileName + forTFinMinutes + "mins" + ".txt");
    }

    for (auto jsonFileName : filePathsToJsonData) {
//        std::cout << "Starting primary function in huntForSignals file\n";

        std::cout << "Opening " << jsonFileName << std::endl;

        std::string minutesTimeframe = jsonFileName;
        // TODO: код, который может привести к сложно-находимым ошибкам при изменении другого кода
        // В дальнейшем я буду использовать minutesTimeframe в ф-и addToCurrentDb, но на данный
        // единственный способ достать TFinMinutes - достать из jsonFileName ("jsonDataFor*mins.txt")
        // Можно было изменить старый код, или придумать другое решение, но я использовал такое, так
        // как ранее не особо пользовался erase и в этот раз захотел попробовать. С другой стороны,
        // это не очень хороший код, так как он зависит от как бы "magic numbers" - если я захочу
        // изменить шаблон названия файла (например, в некоторых случаях вместо *mins.txt хранить
        // *hours.txt), то изменения сюда автоматически не подтянутся, и может произойти
        // сложно находимая ошибка
        minutesTimeframe.erase( minutesTimeframe.find(g_jsonFilename), g_jsonFilename.length() );
        minutesTimeframe.erase( minutesTimeframe.find("For"), 3);
        minutesTimeframe.erase( minutesTimeframe.find("mins.txt"), 8 );

        std::cout << "Gonna call findSignalInFile" << std::endl;
        // Собственно, ищем сигналы в указанном файле
        findSignalInFile(jsonFileName, minutesTimeframe);
    }

//    if (Py_FinalizeEx() < 0) {
//        std::cout << "Py_Finalize went great";
//    }
    std::cout << "Successfully exiting lookForSignals function" << std::endl;

}