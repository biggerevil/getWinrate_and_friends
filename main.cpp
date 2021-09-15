#include <iostream>
#include <vector>
#include <filesystem>
#include <stdlib.h>

#include "constant.h"
#include "createFileWithLinks.h"
#include "downloadFiles.h"
#include "checkOpenStakes.h"
#include "getResultsOfOpenStakes.h"
#include "huntForSignals.h"
#include "rubyPart.h"
#include "stakeWithCurl.h"
//#include "beastWebSocket.h"
//#include "pythonBinaryApi.h"

//    Ruby-part
bool shouldIRunRubyCode = false;
bool willGoHigher = false;
//bool USDJPY_10h_shouldMakeStake = false;
//bool USDJPY_10h_willGoHigher = false;
//bool USDJPY_7h_shouldMakeStake = false;
//bool USDJPY_7h_willGoHigher = false;
//bool EURJPY_1h_shouldMakeStake = false;
//bool EURJPY_1h_willGoHigher = false;
//bool USDCAD_2h_shouldMakeStake = false;
//bool USDCAD_2h_willGoHigher = false;

// 14 сентября 2021
bool EURJPY_15m_shouldMakeStake;
bool EURJPY_15m_willGoHigher;

int main() {
    // Get starting timepoint
    auto start = std::chrono::high_resolution_clock::now();

    // dateTime is "%Y-%m-%d %H:%M:%S"
    std::string currentTime = g_currentTime;

//    makeAStakeInPython("USD/JPY", "5m", 666, true);

    // У нас всегда один и тот же формат, а значит можно спокойно брать данные с конкретных позиций
    int hour = std::stoi(currentTime.substr(11,2));
    int min = std::stoi(currentTime.substr(14,2));

    createAndFillFileWithLinks("links.txt", hour, min);

    // Заметь, из-за особенностей реализации saveFile передаётся только имя, а НЕ путь к файлу.
    std::string jsonFileName = g_jsonFilename;
    std::string pathToFileWithLinks =  "links.txt";

    downloadFromLinksFile(pathToFileWithLinks.c_str(), jsonFileName);

    const char* dbDirectoryName = "dbDir";
    std::filesystem::create_directory(dbDirectoryName);

    std::vector<std::string> vectorIdsOfOpenStakes;
    vectorIdsOfOpenStakes = checkOpenStakes(dbDirectoryName, currentTime.c_str());

//    std::vector<std::string> vectorIdsOfOpenStakes = {"1"};

    getResultsOfOpenStakes(currentDbPath.c_str(),
                           resultsDbPath.c_str(),
                           vectorIdsOfOpenStakes,
                           jsonFileName);

    lookForSignals(pathToFileWithLinks, jsonFileName);

//    std::cout << "Making stake" << std::endl;
//    makeStakeBeast;
//    std::cout << "Made a stake" << std::endl;

//    std::cout << "Start cleaning database from stakes not at **:00-**:02 where ** % 5 = 0" << std::endl;

//    std::cout << "Собираюсь вызвать Ruby-скрипт. Текущее время:\n" << findCurrentTime("%H:%M:%S") << std::endl;
//    make_stake_with_ruby(willGoHigher);
//    std::cout << "Закончил с вызовом Ruby-скрипта. Текущее время:\n" << findCurrentTime("%H:%M:%S") << std::endl;


//    StakeOptions newStake = StakeOptions("USDJPY", "15", "2000", EURJPY_15m_willGoHigher);
//    makeStakeWithSystemCommand(newStake);

//    Make stake with curl
    if (EURJPY_15m_shouldMakeStake == true) {
        StakeOptions newStake = StakeOptions("EURJPY", "15", "2000", EURJPY_15m_willGoHigher);
        makeStakeWithSystemCommand(newStake);
    }
//    if (USDJPY_7h_shouldMakeStake == true) {
//        StakeOptions newStake = StakeOptions("USDJPY", "420", "3000", USDJPY_7h_willGoHigher);
//        makeStakeWithSystemCommand(newStake);
//    }
//    if (EURJPY_1h_shouldMakeStake == true) {
//        StakeOptions newStake = StakeOptions("EURJPY", "60", "3000", EURJPY_1h_willGoHigher);
//        makeStakeWithSystemCommand(newStake);
//    }
//
//    if (USDCAD_2h_shouldMakeStake == true) {
//        StakeOptions newStake = StakeOptions("USDCAD", "120", "3000", USDCAD_2h_willGoHigher);
//        makeStakeWithSystemCommand(newStake);
//    }
//  Не ставлю 10 часов, так как режим ставки при помощи Sprint на in поддерживает максимум 500 минут, а 10 часов - это 600 минут
//    if (USDJPY_10h_shouldMakeStake == true) {
//        StakeOptions newStake = StakeOptions("NZDUSD", "", "150", USDJPY_10h_willGoHigher);
//    }


    std::cout << "And that is the end of main(). Cool" << std::endl;
//    Добавляю несколько строк с точками, чтобы в журнале логов
//    можно было визуально отличить один запуск от другого
    for (int i = 0; i < 5; ++i) {
        std::cout << '.' << std::endl;
    }

    // Get ending timepoint
    auto stop = std::chrono::high_resolution_clock::now();

    // Get duration. Substart timepoints to
    // get durarion. To cast it to proper unit
    // use duration cast method
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

    std::cout << "Time taken by whole program: "
              << duration.count() << " milliseconds" << std::endl;

    return 0;
}
