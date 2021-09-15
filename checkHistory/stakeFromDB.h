//
// Created by c on 12.09.2021.
//

#ifndef CHECKHISTORY_STAKEFROMDB_H
#define CHECKHISTORY_STAKEFROMDB_H

#include <string>
using namespace std;

struct stakeFromDB {
    int id;
    string pairname;
    string dateWhenSet;
    double valueWhenSet;
    string dateWhenExpired;
    double valueWhenExpired;
    string expirePeriod;
    int certain;
    string checkTime;
    bool success;
    int profitPercent;

public:
    friend std::ostream& operator<< (std::ostream &out, const stakeFromDB &stakeForOutput);
};


#endif //CHECKHISTORY_STAKEFROMDB_H
