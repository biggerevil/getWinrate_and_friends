#ifndef WINRATE_CREATEFILEWITHLINKS_H
#define WINRATE_CREATEFILEWITHLINKS_H

#include <string>
#include <fstream>

void createAndFillFileWithLinks(const char* filepath,
                                         int currentHour,
                                         int currentMinute);

#endif //WINRATE_CREATEFILEWITHLINKS_H
