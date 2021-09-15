//
// Created by c on 12.09.2021.
//

#include "stakeFromDB.h"

#include <iostream>

// Overwrite << for using with my struct
std::ostream& operator<< (std::ostream &out, const stakeFromDB &stakeForOutput)
{
    string divider = ", ";
    // Поскольку operator<< является другом класса Point, то мы имеем прямой доступ к членам Point
    out << "stake values: " << stakeForOutput.id << divider << stakeForOutput.pairname << "\n" <<
        stakeForOutput.dateWhenSet << divider << stakeForOutput.valueWhenSet << "\n" <<
        stakeForOutput.dateWhenExpired << divider << stakeForOutput.valueWhenExpired << "\n" <<
        stakeForOutput.expirePeriod << divider << " certain: " << stakeForOutput.certain << divider <<
        "checkTime: " << stakeForOutput.checkTime << "\n" << "success: " << stakeForOutput.success;
    return out;
}
