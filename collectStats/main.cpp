#include <iostream>
#include <ctime>
#include <iomanip>
#include <filesystem>
#include <vector>
#include <sstream>
#include <fstream>

#include "sqlite3.h"
#include "copyDatabase.h"
#include "commandsForDb.h"
#include "daysToCheck.h"
#include "matplotlibcpp.h"

static int callbackToGetPairnames(void* data, int argc, char** argv, char** azColName) {
    std::vector<std::string>* pairnamesVector = reinterpret_cast<std::vector<std::string>*>(data);
    pairnamesVector->push_back(*argv);
    return 0;
}

std::string getCurrentTime() {
    std::string format = "%Y-%m-%d";
    time_t now = time(0);
    // gmtime(&now) returns a pointer to tm struct in UTC
    tm *gmtm = gmtime(&now);
    // Я буду возвращать string, а не tm, потому что для моих задач (1. Записывать SQLite, где можно
    // хранить только TEXT (нет типа DATE); 2. Сравнивать между собой. Если хранить даты в формате
    // YY-MM-DD часы:минуты, то можно сравнивать между собой string в этом формате

    std::stringstream sstream;
    sstream << std::put_time(gmtm, format.c_str());
    return sstream.str();
}

int main() {

    std::vector<std::string> vectorWithDays;
    vectorWithDays = daysToCheck("2020-09-10", "%Y-%m-%d");

    int dayCounter = 0;
    for (auto day : vectorWithDays) {
        std::cout << "Day #" << dayCounter << ": " << day << std::endl;
        dayCounter += 1;
    }

    if (!std::filesystem::exists(pathToCopiedDb)) {
        copyDatabase();
    }

    sqlite3* copyOfResultsDB;
    char* messageError;
    int exit = sqlite3_open(pathToCopiedDb.c_str(), &copyOfResultsDB);

    if (exit) {
        std::cerr << "Error opening copy of results DB - " << sqlite3_errmsg(copyOfResultsDB) << std::endl;
        std::cerr << "Deleting copy" << std::endl;
        deleteCopyOfDatabase();
        std::exit(-1);
    } else {
        std::cout << "Successfully copied!" << std::endl;
    }

    // currentDay = 2020-01-01, надо добавлять пробел и время
    // Хотя можно обойтись и чисто днём. Так тоже работает
    std::string currentDay = getCurrentTime();

    std::string sql = "SELECT DISTINCT PAIRNAME from RESULTSSTAVKI;";
    std::vector<std::string> pairnamesVector;
    exit = sqlite3_exec(copyOfResultsDB, sql.c_str(), callbackToGetPairnames,
                        &pairnamesVector, &messageError);

    if (exit != SQLITE_OK) {
        std::cout << "Error trying to SELECT DISTINCT PAIRNAME. messageError - "
                  << messageError << ", exit = " << exit << std::endl;
        std::exit(-1);
    }

    std::vector<int> certainValues = findPossibleCertainValues(copyOfResultsDB, messageError);
    std::vector<std::string> vectorWithTFs = findPossibleTFs(copyOfResultsDB, messageError);

//    std::cout << "vectorWirhTFs:" << std::endl;
//    for (auto tf : vectorWithTFs) {
//        std::cout << tf << " ";
//    }
//    std::cout << std::endl;

//    std::vector<int> x(vectorWithDays.size());
    std::vector<int> vectorWithAll42(vectorWithDays.size());
    for (int i = 0; i < vectorWithDays.size(); i++) {
        vectorWithAll42.at(i) = 42;
    }

    std::vector<int> vectorWithAll58(vectorWithDays.size());
    for (int i = 0; i < vectorWithDays.size(); i++) {
        vectorWithAll58.at(i) = 58;
    }

    // Чтобы по оси X были только Месяц-День, а не Месяц-День-Год,
    // создаю ещё один вектор без годов
    std::vector<std::string> vectorWithDaysWithoutYear = vectorWithDays;
    for (std::string& forDay : vectorWithDaysWithoutYear) {
        forDay.erase(0, 5);
    }

    // Дело в том, что для графика мне нужны два вектора - дат и винрейта, и
    // у меня будет график для каждой пары. И я мог бы придумать несколько переменных
    // попроще, но я решил использовать одну сложную с объяснением
    //
    // Здесь map<ИмяВалютнойПары, пара<ВекторДат, векторВинрейтов>>
    std::map<std::string, std::pair<std::vector<std::string>,
            std::vector<double>>> pairsWinratesForDays;

    std::map<std::string, std::pair<std::vector<std::string>,
            std::vector<double>>> pairsWinratesForDaysAndTFs;

    std::map<std::string, std::pair<std::vector<std::string>,
            std::vector<double>>> pairsWinratesForDaysAndTFsAndCertain;

//    for (int i = 0; i < 20; i++) {
//        vectorWithDays.erase(vectorWithDays.begin());
//    }

    for (auto forDay : vectorWithDays) {

        std::vector<double> vectorWithWinratesOfCurrentDay(vectorWithDays.size());
        std::cout << "Gonna collect stats for day = " << forDay << std::endl;

        for (std::string& pairname : pairnamesVector) {
            // Убираю из pairname '/', иначе не получится сохранить график, так как в пути
            // для сохранения будет /, не значащий переход в директорию
            // (то есть могло бы быть  ./1111-01-01,USD/CAD.png, а с помощью
            // следующих строк я делаю ./1111-01-01,USDCAD.png, и можно сохранять)
            std::string pairnameForPath = pairname;
            pairnameForPath.erase(
                    std::remove(pairnameForPath.begin(), pairnameForPath.end(), '/'),
                    pairnameForPath.end());
            if (pairnameForPath != "USDJPY") {
                std::cout << "#0 Continues pairname = " << pairnameForPath << " for day " << forDay << std::endl;
                continue;
            }
            std::cout << "#0 Found USDJPY!" << std::endl;
            // Здесь удаляю год. Учитывая мой vectorWithDaysWithoutYear здесь происходит
            // как бы дублирование кода, но я оставлю это так, так как не вижу смысла
            // сейчас менять это всё и это не оказывает особого влияния.
            // По идее должно сработать, если я цикл с forDay сменю на int i = 0; ...
            // и сразу же после этого напишу forDay = vectorWithDays[i], а здесь буду писать
            // vectorWithDaysWithoutYear[i]. Тогда не придётся менять весь следующий код
            // и дублирования не будет.
            std::string forDayForPath = forDay;
            forDayForPath.erase(0, 5);

            std::string plotOfCurrentDayPath = "screensDir/" + pairnameForPath + ".png";

//            matplotlibcpp::plot(vectorWithDaysWithoutYear, vectorWithAll42, "r-");
//            matplotlibcpp::plot(vectorWithDaysWithoutYear, vectorWithAll58, "r-");
//            matplotlibcpp::ylim(0, 100);
//
//            matplotlibcpp::save(plotOfCurrentDayPath);

            int losesForThisDayAndPair = countLosesForPairTfAndCertain(
                    copyOfResultsDB, messageError, pairname, "-1", -1, forDay);
            int allStakesForThisDayAndPair = countAllStakesOfPairTfAndCertain(
                    copyOfResultsDB, messageError, pairname, "-1", -1, forDay);

            double winrateForDay = (allStakesForThisDayAndPair - losesForThisDayAndPair) /
                    (double)allStakesForThisDayAndPair;
            winrateForDay *= 100;

            if (pairsWinratesForDays.count(pairname) == 0) {
                std::vector<std::string> stringVector;
                std::vector<double>         doubleVector;
                pairsWinratesForDays[pairname] = std::make_pair(stringVector, doubleVector);
            }

            pairsWinratesForDays[pairname].first.push_back(forDayForPath);
            pairsWinratesForDays[pairname].second.push_back(winrateForDay);


            // PAIRNAME
//            std::cout << "**" << pairname << "**" << std::endl;
            // loses AND in all
            int generalLoses = countLosesForPairTfAndCertain(copyOfResultsDB, messageError, pairname);
//            std::cout << "loses\t = " <<
//                      generalLoses << std::endl;
            int allStakesCount = countAllStakesOfPairTfAndCertain(copyOfResultsDB, messageError, pairname);
//            std::cout << "in all\t = " <<
//                      allStakesCount << std::endl;



            // winrate AND reverse
            double winrate = generalLoses / (double)allStakesCount;
            winrate *= 100;
            double reverseWinrate = (allStakesCount - generalLoses) / (double)allStakesCount;
            reverseWinrate *= 100;
            if (winrate >= 57 || winrate <= 43) {
//                std::cout << "winrate\t = **" << winrate << "**" << std::endl;
//                std::cout << "reverse\t = **" << reverseWinrate << "**" << std::endl;
            } else {
//                std::cout << "winrate\t = " << winrate << std::endl;
//                std::cout << "reverse\t = " << reverseWinrate << std::endl;
            }

            for (auto timeframe : vectorWithTFs) {
                if (timeframe != "5m") {
                    std::cout << "#1 Continues timeframe = " << timeframe << std::endl;
                    continue;
                }
                std::cout << "#1 Found 5m!" << std::endl;
//                std::cout << std::endl;
//                std::cout << timeframe << std::endl;
                int losesForTf = countLosesForPairTfAndCertain(copyOfResultsDB, messageError,
                                                               pairname, timeframe,
                                                               -1, forDay);
                int allStakesForTf = countAllStakesOfPairTfAndCertain(copyOfResultsDB, messageError,
                                                                      pairname, timeframe,
                                                                      -1, forDay);
                if (allStakesForTf == 0) {
//                    std::cout << "There are 0 stakes for this timeframe" << std::endl;
                    continue;
                }

                double winrateForTf = (allStakesForTf - losesForTf) / (double) allStakesForTf;
                winrateForTf *= 100;
                double reverseWinrateForTf = losesForTf / (double)allStakesForTf;
                reverseWinrateForTf *= 100;
//                std::cout << "% выигрышей за последний день\t = " << winrateForTf << std::endl;
//                std::cout << "% проигрышей за последний день\t = " << reverseWinrateForTf << std::endl;
//                std::cout << "Всего ставок = " << allStakesForTf << " ("
//                          << (allStakesForTf - losesForTf) << "/" << allStakesForTf << ")" << std::endl;


                // Информация для графиков. Теперь в качестве ключа имеем ещё и timeframe
                if (pairsWinratesForDaysAndTFs.count(pairname + timeframe) == 0) {
                    std::vector<std::string> stringVector;
                    std::vector<double>         doubleVector;
                    pairsWinratesForDaysAndTFs[pairname + timeframe] = std::make_pair(stringVector, doubleVector);
                }

                pairsWinratesForDaysAndTFs[pairname + timeframe].first.push_back(forDayForPath);
                pairsWinratesForDaysAndTFs[pairname + timeframe].second.push_back(winrateForTf);



                for (auto certain : certainValues) {
                    if (certain != 2) {
                        std::cout << "#1 Continues certain = " << certain << std::endl;
                        continue;
                    }
                    std::cout << "#1 Found certain 2!" << std::endl;
                    int losesForTfAndCertain = countLosesForPairTfAndCertain(copyOfResultsDB, messageError,
                                                                             pairname, timeframe,
                                                                             certain, forDay);
                    int allStakesForTfAndCertain = countAllStakesOfPairTfAndCertain(copyOfResultsDB, messageError,
                                                                                    pairname, timeframe,
                                                                                    certain, forDay);
                    if (allStakesForTfAndCertain == 0) {
//                        std::cout << "There are 0 stakes for this timeframe + certain " <<
//                                  certain << std::endl;
                        continue;
                    }

                    double winrateForTfAndCertain = (allStakesForTfAndCertain -
                                                     losesForTfAndCertain) / (double) allStakesForTfAndCertain;
                    winrateForTfAndCertain *= 100;

                    if (pairsWinratesForDaysAndTFsAndCertain.count(pairname + timeframe + std::to_string(certain)) == 0) {
                        std::vector<std::string> stringVector;
                        std::vector<double>         doubleVector;

                        pairsWinratesForDaysAndTFsAndCertain[pairname + timeframe + std::to_string(certain)] = std::make_pair(stringVector, doubleVector);
                    }

                    pairsWinratesForDaysAndTFsAndCertain[pairname + timeframe + std::to_string(certain)].first.push_back(forDayForPath);
                    pairsWinratesForDaysAndTFsAndCertain[pairname + timeframe + std::to_string(certain)].second.push_back(winrateForTfAndCertain);
//                double reverseWinrateForTfAndCertain = losesForTfAndCertain / (double) allStakesForTfAndCertain;
//                reverseWinrateForTf *= 100;
//                    std::cout << "% проигрышей с certain = " << certain << "\t: " << winrateForTfAndCertain
//                              << "% " << "(" << (allStakesForTfAndCertain - losesForTfAndCertain) <<
//                              "/" << allStakesForTfAndCertain << ")" << std::endl;

                    // TODO: Выделять жирным, если кол-во ставок больше 5 (или 7-8, или 10, или даже 15) И
                    //  если винрейт больше 58% (60%, 62%, 63%)
                    // Проверяем, timeframe - это минутный или часовой/дневной. Если минутный, то в нём
                    // есть буква m (я так обозначаю минутные). И это значит, что собирать статистику стоит
                    // только если там больше 4 ставок при текущем certain
                    // TODO: дублирование кода, возможность сократить, если переписать условие с
                    //  'm' и > 4
                    if (timeframe.find('m') != std::string::npos) {
                        if (allStakesForTfAndCertain > 4) {
                            if (winrateForTfAndCertain > 59 || winrateForTfAndCertain < 41) {
                                // Записывать надо, например,
                                // ВалютнаяПара - Тф - certain - винрейт (побед/проигрышей или побед/всего ставок
                                // или проигрышей/всего ставок)
                                if (!std::filesystem::exists(pathToDirWithNotableFiles)) {
//                                    std::cout << "Gonna try to create directory" << std::endl;
                                    std::filesystem::create_directory(pathToDirWithNotableFiles);
                                }
                                std::string pathToCurrentDayFile = pathToDirWithNotableFiles + "/" + forDay + ".txt";
                                if (!std::filesystem::exists(pathToCurrentDayFile)) {
                                    std::ofstream currentDayFstream(pathToCurrentDayFile);
                                    currentDayFstream.close();
                                }
                                std::ofstream currentDayFstream;
                                currentDayFstream.open(pathToCurrentDayFile.c_str(), std::ios::app);
                                currentDayFstream << pairname << ",\t" << timeframe << ",\t" <<
                                                  certain << ",\t" << winrateForTfAndCertain << ",\t" <<
                                                  (allStakesForTfAndCertain - losesForTfAndCertain) << "/" <<
                                                  allStakesForTfAndCertain << "\n";
                                currentDayFstream.close();
                            }
                        }
                    }
                    // В этот блок else мы попадаем только если timeframe у нас часовой или дневной
                    else {
                        if (winrateForTfAndCertain > 59 || winrateForTfAndCertain < 41) {
                            // Записывать надо, например,
                            // ВалютнаяПара - Тф - certain - винрейт (побед/проигрышей или побед/всего ставок
                            // или проигрышей/всего ставок)
                            if (!std::filesystem::exists(pathToDirWithNotableFiles)) {
                                std::filesystem::create_directory(pathToDirWithNotableFiles);
                            }
                            std::string pathToCurrentDayFile = pathToDirWithNotableFiles + "/" + forDay + ".txt";
                            if (!std::filesystem::exists(pathToCurrentDayFile)) {
                                std::ofstream currentDayFstream(pathToCurrentDayFile);
                                currentDayFstream.close();
                            }
                            std::ofstream currentDayFstream;
                            currentDayFstream.open(pathToCurrentDayFile.c_str(), std::ios::app);
                            currentDayFstream << pairname << ",\t" << timeframe << ",\t" <<
                                              certain << ",\t" << winrateForTfAndCertain << ",\t" <<
                                              (allStakesForTfAndCertain - losesForTfAndCertain) << "/" <<
                                              allStakesForTfAndCertain << "\n";
                            currentDayFstream.close();
                        }
                    }

                }
            }
//            std::cout << std::endl << std::endl;
        }

    }

    std::cout << "Start making images for pairs" << std::endl;
    for (auto pairname : pairnamesVector) {
        if (pairname != "USDJPY") {
            std::cout << "#1 Continues pairname = " << pairname << std::endl;
            continue;
        }
        std::cout << "#1 Found USDJPY!" << std::endl;
        matplotlibcpp::figure_size(1920, 1080);

        matplotlibcpp::plot(vectorWithDaysWithoutYear, vectorWithAll42, "r-");
        matplotlibcpp::plot(vectorWithDaysWithoutYear, vectorWithAll58, "r-");
        matplotlibcpp::ylim(0, 100);







        matplotlibcpp::plot(
                pairsWinratesForDays[pairname].first,
                pairsWinratesForDays[pairname].second);

        std::string pairnameForPath = pairname;
        pairnameForPath.erase(
                std::remove(pairnameForPath.begin(), pairnameForPath.end(), '/'),
                pairnameForPath.end());

        matplotlibcpp::title(pairnameForPath + ", all");
        std::string plotOfCurrentDayPath = "screensDir/"
                                            + pairnameForPath
                                            + ".png";

        matplotlibcpp::save(plotOfCurrentDayPath);
        matplotlibcpp::close();
    }

    // Тут идёт как бы дублирование кода, цикл с timeframe можно было бы добавить в
    // цикл выше, где только pairname. Не добавляю сейчас, чтобы код был логически разделён
    // и чтобы было проще ориентироваться
    std::cout << "Start making images for timeframes" << std::endl;
    for (auto pairname : pairnamesVector) {
        if (pairname != "USDJPY") {
            std::cout << "#2 Continues pairname = " << pairname << std::endl;
            continue;
        }
        std::cout << "#2 Found USDJPY!" << std::endl;
        for (auto timeframe : vectorWithTFs) {
            if (timeframe != "5m") {
                std::cout << "#2 Continues timeframe = " << timeframe << std::endl;
                continue;
            }
            std::cout << "#2 Found 5m!" << std::endl;
            matplotlibcpp::figure_size(1920, 1080);

            matplotlibcpp::plot(vectorWithDaysWithoutYear, vectorWithAll42, "r-");
            matplotlibcpp::plot(vectorWithDaysWithoutYear, vectorWithAll58, "r-");
            matplotlibcpp::ylim(0, 100);

            matplotlibcpp::plot(
                    pairsWinratesForDaysAndTFs[pairname + timeframe].first,
                    pairsWinratesForDaysAndTFs[pairname + timeframe].second);

            std::string pairnameForPath = pairname;
            pairnameForPath.erase(
                    std::remove(pairnameForPath.begin(), pairnameForPath.end(), '/'),
                    pairnameForPath.end());
            matplotlibcpp::title(pairnameForPath + " - " + timeframe);

            std::string plotOfCurrentDayPath = "screensDir/"
                                               + pairnameForPath + "-"
                                               + timeframe
                                               + ".png";

            matplotlibcpp::save(plotOfCurrentDayPath);
            matplotlibcpp::close();
        }
    }

    std::cout << "Start making images for certains" << std::endl;
    for (auto pairname : pairnamesVector) {
        // Убираю из pairname '/', иначе не получится сохранить график, так как в пути
        // для сохранения будет /, не значащий переход в директорию
        // (то есть могло бы быть  ./1111-01-01,USD/CAD.png, а с помощью
        // следующих строк я делаю ./1111-01-01,USDCAD.png, и можно сохранять)
        std::string pairnameForPath = pairname;
        pairnameForPath.erase(
                std::remove(pairnameForPath.begin(), pairnameForPath.end(), '/'),
                pairnameForPath.end());
        if (pairnameForPath != "USDJPY") {
            std::cout << "#3 Continues pairname = " << pairnameForPath << std::endl;
            continue;
        }
        std::cout << "#3 Found USDJPY!" << std::endl;
        for (auto timeframe : vectorWithTFs) {
            if (timeframe != "5m") {
                std::cout << "#3 Continues timeframe = " << timeframe << std::endl;
                continue;
            }
            std::cout << "#3 Found 5m!" << std::endl;
            for (auto certain : certainValues) {
                if (certain != 2) {
                    std::cout << "#3 Continues certain = " << certain << std::endl;
                    continue;
                }
                std::cout << "#3 Found certain 2!" << std::endl;

                matplotlibcpp::figure_size(1920, 1080);

                matplotlibcpp::plot(vectorWithDaysWithoutYear, vectorWithAll42, "r-");
                matplotlibcpp::plot(vectorWithDaysWithoutYear, vectorWithAll58, "r-");
                matplotlibcpp::ylim(0, 100);

                matplotlibcpp::plot(
                        pairsWinratesForDaysAndTFsAndCertain[pairname + timeframe + std::to_string(certain)].first,
                        pairsWinratesForDaysAndTFsAndCertain[pairname + timeframe + std::to_string(certain)].second);

//                for (auto value : pairsWinratesForDaysAndTFsAndCertain) {
//                    std::cout << value.first << std::endl;
//                }

                std::string pairnameForPath = pairname;
                pairnameForPath.erase(
                        std::remove(pairnameForPath.begin(), pairnameForPath.end(), '/'),
                        pairnameForPath.end());



                matplotlibcpp::title(pairnameForPath + " - " + timeframe +
                                        " - certain=" + std::to_string(certain));

                std::string plotOfCurrentDayPath = "screenDir/"
                                                   + pairnameForPath + "-"
                                                   + timeframe + "-"
                                                   + std::to_string(certain)
                                                   + ".png";

//                Had a mistake в день, когда был 4 коммит РоРа
//                try {
//                    matplotlibcpp::save(plotOfCurrentDayPath);
//                } catch (std::exception& exc) {
//                    std::cerr << "exc = " << exc.what() << std::endl;
//                }
                matplotlibcpp::save(plotOfCurrentDayPath);
                matplotlibcpp::close();

            }
        }
    }

//    dayCounter = 0;
//    for (auto day : vectorWithDays) {
//        std::cout << "Day #" << dayCounter << ": " << day << std::endl;
//        dayCounter += 1;
//    }

    std::cout << "Successful exit from main()" << std::endl;
//    deleteCopyOfDatabase();

    return 0;
}
