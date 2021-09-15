#ifndef WINRATE_DOWNLOADFILES_H
#define WINRATE_DOWNLOADFILES_H

void downloadFromLinksFile(const char* pathToFileWithLinks, std::string saveFilePath);
void downloadFileFromUrl(const char* saveFilePath, const char* urlToDownload);

#endif //WINRATE_DOWNLOADFILES_H
