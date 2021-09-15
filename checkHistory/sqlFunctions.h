//
// Created by c on 12.09.2021.
//

#ifndef CHECKHISTORY_SQLFUNCTIONS_H
#define CHECKHISTORY_SQLFUNCTIONS_H

#include <list>

#include "stakeFromDB.h"


void writeDataToList(list<stakeFromDB> &listOfStakesFromDB,
                     string pairname,
                     string timeframe,
                     int certain,
                     int roundTime,
                     bool &revert);

#endif //CHECKHISTORY_SQLFUNCTIONS_H
