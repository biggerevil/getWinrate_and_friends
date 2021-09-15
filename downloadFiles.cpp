#include <curl/curl.h>
#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>

#include "downloadFiles.h"

// TODO: в downloadFile используется файл для сохранения, но код для этого совсем не похож на код из
// createFileWithLinks.cpp. Надо бы привести всё это к единому стандарту, чтобы потом работодатель
// не спрашивал, почему это у меня для одной и той же задачи используется код разных стилей
// (ну то есть дело не только в работодателе, конечно, в любом случае не стоит писать код одной
// и той же программы в разных стилях)

void downloadFileFromUrl(const char* saveFilePath, const char* urlToDownload) {
    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    if(curl) {

        std::cout << "urlToDownload = " << urlToDownload << std::endl;
        curl_easy_setopt(curl, CURLOPT_URL, urlToDownload);

        /* example.com is redirected, so we tell libcurl to follow redirection */
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        // You get 403 Forbidden if you don't provide user agent
        curl_easy_setopt(curl, CURLOPT_USERAGENT,
                         "Mozilla/5.0 (Windows NT 6.1; rv:19.0) Gecko/20100101 Firefox/19.0");

        // We'll write curl result in a file
        FILE* file = fopen(saveFilePath, "w");
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);

        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if(res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));

        /* always cleanup */
        curl_easy_cleanup(curl);

        // We should close the file to save curl result
        fclose(file);
    }
}

void downloadFromLinksFile(const char* pathToFileWithLinks, std::string saveFileNAME) {
    std::ifstream fileReadStream(pathToFileWithLinks);
//    Checking stuff
//    std::cout << "[downloadFiles.cpp] pathToFileWithLinks = " << pathToFileWithLinks << std::endl;

    // Если мы не можем открыть этот файл для чтения его содержимого
    if (!fileReadStream)
    {
        // То выводим следующее сообщение об ошибке и выполняем exit()
        std::cerr << "Не удаётся прочитать файл со ссылками. Выполняю exit(1)" << std::endl;
        exit(1);
    }
    
    std::string modifiedSaveFilePath;
    // Пока есть данные, которые мы можем прочитать
    std::string urlFromFile;
    while(fileReadStream >> urlFromFile) {
//        Checking stuff
//        std::cout << "[downloadFiles.cpp] urlFromFile = " << urlFromFile << std::endl;

        // Раньше было просто while(fileReadStream), но тогда читалась последняя пустая строчка файла,
        // от чего curl завершал программу из-за попытки открытия пустого URL. Если использовать
        // while(fileReadStream >> urlFromFile), то такой ошибки не возникает
        // fileReadStream >> urlFromFile;

        // Надо бы добавлять в конец файла (куда будем сохранять скачанное) указание ТФ
        // (5 мин, 15 мин, 30 мин, ...), для которого скачаны данные. Ссылка, по которой я
        // скачиваю файлы, оканчивается на timeframe=xxx, где xxx - искомое значение (в
        // секундах, то есть 5 минут = 300) xxx на данный момент не больше 5 символов, я с
        // запасом начинаю искать символ равно с 8 символа с конца ссылки. Нахожу итератор
        // на '=', и беру xxx при помощи std::string(итератор на '=' + 1, итератор на конец ссылки)
        auto equalOperatorPosition = std::find(urlFromFile.end() - 8,
                                               urlFromFile.end(), '=');
        std::string TFinSeconds = std::string(equalOperatorPosition + 1, urlFromFile.end());
        // А затем я превращаю TFinSeconds в int, делю на 60, чтобы получить минуты,
        // снова превращаю в string, чтобы сложить с For и вставить в имя файла
        std::string forTFinMinutes = "For" + std::to_string(std::stoi(TFinSeconds) / 60);

        modifiedSaveFilePath = saveFileNAME + forTFinMinutes + "mins" + ".txt";
        downloadFileFromUrl(modifiedSaveFilePath.c_str(), urlFromFile.c_str());
    }

    return;
}