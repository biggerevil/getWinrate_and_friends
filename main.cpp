#include <iostream>
#include <list>
#include <queue>

using namespace std;

#include "sqlite3.h"

#include "constants.h"
#include "stakeFromDB.h"
#include "sqlFunctions.h"

// В рублях
double DEPOSIT = 10000;
double newStakeSum = 60;
bool fixedNewStake = false;

//string pairname = "USD/JPY";
//string timeframe = "5m";
//int certain = 2;

int madeStakesCount = 0;
int notMadeStakesCount = 0;
int wonStakes = 0;
int lostStakes = 0;

//От 4 до 500 минут до 5000р - 79%.
//От 4 до 500 минут от 5000р - 85%.
//За 3 минуты до и после начала часа выплаты равны 60% (4-9 и 17-23 по мск).
int profitPercent(const stakeFromDB& newStake) {
    int hourOfNewStake = stoi(newStake.dateWhenSet.substr(11, 2));
    int minutesOfNewStake = stoi(newStake.dateWhenSet.substr(14, 2));
//    На данный момент беру обычную сумму ставки
    if ((hourOfNewStake >= 4 and hourOfNewStake <= 9) or (hourOfNewStake >= 17 and hourOfNewStake <= 23)) {
        if ((minutesOfNewStake - 3 >= 57) or (minutesOfNewStake <= 3)) {
            return 60;
        }
    }
    double sumOfNewStake = newStakeSum;
//    Я не уверен, что здесь должно быть <=, а не просто <, но, думаю, пусть будет так
    if (sumOfNewStake <= 5000) {
        return 79;
    }
    return 85;
}

void checkAndRemoveOldestStake(stakeFromDB& newStake, queue<stakeFromDB>& currentStakes) {

    if (newStake.dateWhenSet > currentStakes.front().dateWhenExpired) {

//                int hourOfNewStakeInGMT3 = stoi(newStake.dateWhenSet.substr(11, 2));
//                int minutesOfNewStakeInGMT3 = stoi(newStake.dateWhenSet.substr(14, 2));
//                if ((hourOfNewStakeInGMT3 == 17) and (minutesOfNewStakeInGMT3 == 50) ){
//                    cout << "Break" << endl;
//                    cout << "currentStakes.front(): " << currentStakes.front() << endl;
//                    cout << "newStake: " << newStake << endl;
//                }

//        cout << "Сравниваю время:\n" << newStake.dateWhenSet << " > " << currentStakes.front().dateWhenExpired << endl;
        stakeFromDB doneStake = currentStakes.front();
//                Remove done stake from currentStakes
        currentStakes.pop();

        if (doneStake.success) {
//                    cout << "Выигрыш" << endl;
            wonStakes++;
            DEPOSIT += newStakeSum;
            DEPOSIT += newStakeSum * (doneStake.profitPercent / 100.0);
        } else if (not doneStake.success) {
//                    cout << "Проигрыш" << endl;
            lostStakes++;
        } else {
            cout << "У doneStake что-то не так с success. Похоже, это не 1 и не 0.";
            cerr << "У doneStake что-то не так с success. Похоже, это не 1 и не 0.";
            std::abort();
        }
    }
}

int main() {
    std::cout << "Hello, World!" << std::endl;

    const vector<string> allPairnames{"AUD/USD", "GBP/USD", "EUR/CHF", "EUR/USD", "EUR/JPY", "USD/JPY", "USD/CAD"};
    const vector<string> allTimeframes{"5m", "15m", "30m", "1h", "2h", "5h", "7h", "10h", "1d", "2d"};
    const vector<int> allCertainValues{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    const vector<int> allRoundTimes{0, 1, 2, 3, 4};

    for (string pairname : allPairnames) {
        for (string timeframe : allTimeframes) {
            for (int certain : allCertainValues) {
//                for (int roundTime : allRoundTimes) {
//              Этот код с revert (и toRevert впоследствии) это жёстко, мда.......
//              Делаю так, потому что не хочу париться и определять, какие условия надо поворачивать, а какие нет
//              Поэтому я в итоге переворачиваю всё (можно было продублировать код, но так больше вероятность
//              ошибки и больше вероятность запутаться
                bool revert = false;
                for (int i = 0; i < 2; i++) {
                    madeStakesCount = 0;
                    notMadeStakesCount = 0;
                    wonStakes = 0;
                    lostStakes = 0;

                    DEPOSIT = 10000;
                    newStakeSum = 60;
                    fixedNewStake = false;

                    list<stakeFromDB> listOfStakesFromDB;

                    writeDataToList(listOfStakesFromDB, pairname, timeframe, certain, -1, revert);
//                    writeDataToList(listOfStakesFromDB, pairname, timeframe, certain, roundTime);

                    if (listOfStakesFromDB.empty()) {
                        cout << "Пустота................." << endl;
                        continue;
                    }

//                    cout << listOfStakesFromDB.back() << endl;

                    queue<stakeFromDB> currentStakes;

//    TODO: добавить выборку по паре, ТФ и мб certain с rount time (последние два можно уже позже)
//      в изначальный sql-запрос


                    for (stakeFromDB &newStake: listOfStakesFromDB) {

                        if (not currentStakes.empty()) {
                            checkAndRemoveOldestStake(newStake, currentStakes);
                        }

                        if (currentStakes.size() >= MaximumQueueSize) {
//                            cout << "Не делаю ставку из-за ограничения на 5 ставок. "
//                                    "currentStakes.size(): " << currentStakes.size() << endl;
                            notMadeStakesCount++;
                            continue;
                        } else {
//                            cout << "Ограничение не выполняется. "
//                                    "currentStakes.size() = " << currentStakes.size() << endl;
                        }

//        Get hour of new stake (with my format this substr takes only hour)
//        These are the rules of in - you can't make new stakes between 0 and 4 hours (am, not pm)
                        int hourOfNewStakeInGMT3 = stoi(newStake.dateWhenSet.substr(11, 2))
                                                   //        And immediately convert it to GMT +3 because these hours by default in UTC and in works in GMT +3
                                                   + 3;
                        if (hourOfNewStakeInGMT3 < 4 and hourOfNewStakeInGMT3 > 0) {
//                            cout << "Не делаю ставку из-за времени. Час ставки: " << hourOfNewStakeInGMT3 << endl;
                            notMadeStakesCount++;
                            continue;
                        }

                        if (DEPOSIT - newStakeSum < 0) {
                            cout << "Баланс при новой ставке уйдёт в минус!";
                            if (currentStakes.empty()) {
                                cout << "currentStakes опустошён, денег нет, тикаю." << endl;
                                break;
                            }
                            continue;
                        }

                        newStake.profitPercent = profitPercent(newStake);
                        DEPOSIT -= newStakeSum;
                        currentStakes.push(newStake);
//        Можно попробовать брать 1% от депозита (это же 1% берётся, верно?)
                        if (not fixedNewStake) {
                            if ((DEPOSIT / 100) > 65 and (DEPOSIT / 100) < 25'000) {
                                newStakeSum = DEPOSIT / 100;
                            }
                        }
                        madeStakesCount++;
                    }

//    На данный момент остаётся ещё 5 не сделанных ставок. Они остаются в currentStakes

                    cout << "\n\n  Вывод статистики: \n" << endl;
                    cout << "Всего записей в БД по выбранным условиям: " << listOfStakesFromDB.size() << endl;
                    cout << "Сделано ставок: \t" << madeStakesCount << endl;
                    cout << "Не сделано ставок: \t" << notMadeStakesCount << endl;
                    cout << "Выиграно ставок: \t" << wonStakes << endl;
                    cout << "Проиграно ставок: \t" << lostStakes << endl;

                    cout << fixed << "\n Депозит: " << DEPOSIT << endl;
                    if (DEPOSIT > 11'500) {
                        cout << "Проверь!" << endl;
                    }
                }
//                }
            }
        }
    }

//    list<stakeFromDB> listOfStakesFromDB;
//
//    //int roundTime = 1;
//
////    writeDataToList(listOfStakesFromDB, pairname, timeframe);
//    writeDataToList(listOfStakesFromDB, pairname, timeframe, certain);

//    cout << listOfStakesFromDB.back() << endl;
//
//    queue<stakeFromDB> currentStakes;
//
////    TODO: добавить выборку по паре, ТФ и мб certain с rount time (последние два можно уже позже)
////      в изначальный sql-запрос
//
//
//    for (stakeFromDB& newStake : listOfStakesFromDB) {
//
//        if (not currentStakes.empty()) {
//            checkAndRemoveOldestStake(newStake, currentStakes);
//        }
//
//        if (currentStakes.size() >= MaximumQueueSize) {
//            cout << "Не делаю ставку из-за ограничения на 5 ставок. "
//                    "currentStakes.size(): " << currentStakes.size() << endl;
//            notMadeStakesCount++;
//            continue;
//        } else {
//            cout << "Ограничение не выполняется. "
//                    "currentStakes.size() = " << currentStakes.size() << endl;
//        }
//
////        Get hour of new stake (with my format this substr takes only hour)
////        These are the rules of in - you can't make new stakes between 0 and 4 hours (am, not pm)
//        int hourOfNewStakeInGMT3 = stoi(newStake.dateWhenSet.substr(11, 2))
////        And immediately convert it to GMT +3 because these hours by default in UTC and in works in GMT +3
//                + 3;
//        if (hourOfNewStakeInGMT3 < 4 and hourOfNewStakeInGMT3 > 0) {
//            cout << "Не делаю ставку из-за времени. Час ставки: " << hourOfNewStakeInGMT3 << endl;
//            notMadeStakesCount++;
//            continue;
//        }
//
//        if (DEPOSIT - newStakeSum < 0) {
//            cout << "Депозит при новой ставке опустится ниже 0!";
//            continue;
//        }
//
//        newStake.profitPercent = profitPercent(newStake);
//        DEPOSIT -= newStakeSum;
//        currentStakes.push(newStake);
////        Можно попробовать брать 1% от депозита (это же 1% берётся, верно?)
//        if (not fixedNewStake) {
//            if ((DEPOSIT / 100) > 65 and (DEPOSIT / 100) < 25'000) {
//                newStakeSum = DEPOSIT / 100;
//            }
//        }
//        madeStakesCount++;
//    }
//
////    На данный момент остаётся ещё 5 не сделанных ставок. Они остаются в currentStakes
//
//    cout << "\n\n  Вывод статистики: \n" << endl;
//    cout << "Всего записей в БД по выбранным условиям: " << listOfStakesFromDB.size() << endl;
//    cout << "Сделано ставок: \t" << madeStakesCount << endl;
//    cout << "Не сделано ставок: \t" << notMadeStakesCount << endl;
//    cout << "Выиграно ставок: \t" << wonStakes << endl;
//    cout << "Проиграно ставок: \t" << lostStakes << endl;
//
//    cout << fixed << "\n Депозит: " << DEPOSIT;
    return 0;
}
