//
// Created by c on 17.09.2020.
//

#include "daysToCheck.h"
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

// Эта функция будет формировать массив строк с днями,
// статистику по которым надо собрать. На вход ф-я получает первый день,
// и затем добавляет дни, пока не дойдёт до вчерашнего дня
// (вчерашнего, так как статистика будет собираться за целые дни,
// а значит, сегодняшний день брать не следует, он же ещё не прошёл)
std::vector<std::string> daysToCheck(std::string firstDay, std::string format) {

    // В векторе будут храниться все даты, для которых надо собирать статистику
    // Даты будут храниться в string, чтобы потом можно было сразу подставить в sql-запрос
    // Сразу же добавляем первый день
    std::vector<std::string> vectorToReturn = {};
    vectorToReturn.push_back(firstDay);

    // Здесь логика какая:
    // Создадим в виде tm первый день (firstDay), чтобы можно было легко сравнивать
    // с другими датами (которые тоже будут в tm)
    // И создадим tm вчерашнего дня (то есть создадим сегодняшний и отнимем один день)
    // И просто сделаем цикл от первого дня до вчерашнего. На каждем шаге цикла будем
    // добавлять string в вектор
    std::tm firstDayInTm = {};
    std::istringstream ss(firstDay);
    ss >> std::get_time(&firstDayInTm, format.c_str());
    std::mktime(&firstDayInTm);

    std::tm* yesterdayInTm = {};
    std::time_t now = std::time(0);
    // Из сегодняшнего дня вычитаем день, чтобы получить вчерашний
    now -= 86400;
    yesterdayInTm = gmtime(&now);

    while (firstDayInTm.tm_mday != yesterdayInTm->tm_mday
            ||
           firstDayInTm.tm_mon  != yesterdayInTm->tm_mon)
    {
        firstDayInTm.tm_mday += 1;
        std::mktime(&firstDayInTm);
        // Субботы и воскресенья нет смысла проверять - по выходным обычная биржа не работает
        // В tm (структура в C++) 6 - это суббота. 0 - это воскресенье
        if (firstDayInTm.tm_wday == 6 || firstDayInTm.tm_wday == 0)
        {
            continue;
        }
        std::ostringstream oss;
        oss << std::put_time(&firstDayInTm, format.c_str());
        vectorToReturn.push_back(oss.str());
    }

    return vectorToReturn;
}