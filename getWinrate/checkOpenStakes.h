#ifndef WINRATE_CHECKOPENSTAKES_H
#define WINRATE_CHECKOPENSTAKES_H

void fillDbWhileDevelopingProgram(const char* currentDbPath);
std::vector<std::string> checkOpenStakes(const char* dbDir, const char* currentTime);

#endif //WINRATE_CHECKOPENSTAKES_H
