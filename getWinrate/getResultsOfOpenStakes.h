//
// Created by c on 17.08.2020.
//

#ifndef GETWINRATE_GETRESULTSOFOPENSTAKES_H
#define GETWINRATE_GETRESULTSOFOPENSTAKES_H

void getResultsOfOpenStakes(const char* pathToCurrentDb,
                            const char* pathToResultsDb,
                            std::vector<std::string> vectorIdsOfOpenStakes,
                            std::string fileNameWithJson);

#endif //GETWINRATE_GETRESULTSOFOPENSTAKES_H
