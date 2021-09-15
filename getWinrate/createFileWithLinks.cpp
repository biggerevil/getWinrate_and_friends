#include <iostream>
#include <filesystem>

#include "createFileWithLinks.h"
#include "credentials.h"

void checkFileExistsAndDelete(const char* filepath) {
    bool fileExists = std::filesystem::exists(filepath);
    if (fileExists) {
        int removeCode = remove(filepath);
        if (removeCode != 0) {
            std::cerr << "Tried to delete file with links - didn't success. "
                         "Code returned by remove() is " << removeCode << ". Executing exit(1)" << std::endl;
            std::exit(1);
        }
    }
}

// For now didn't have to use currentHour
void fillFileWithLinks(const char* filepath,
                               int currentHour,
                               int currentMinute) {

    // Открываем файл
    std::ofstream fileWithLinks(filepath);
    // Если не можем открыть файл для записи данных в него
    if (!fileWithLinks) {
        // То выводим сообщение об ошибке и выполняем exit()
        std::cerr << "Не удалось открыть файл для записи. Выполняю exit(1)\n";
        exit(1);
    }

    std::string baseLink = credentialsBaseLink;
    fileWithLinks << baseLink + "300\n";
    fileWithLinks << baseLink + "900\n";
    fileWithLinks << baseLink + "1800\n";
    fileWithLinks << baseLink + "3600\n";
    fileWithLinks << baseLink + "18000\n";
    fileWithLinks << baseLink + "86400\n";

    // Закомментировал строчки ниже, которые в зависимости от минуты
    // (там 01 минута или 15 минута или 30 или 35) загружали разные
    // файлы. Закомментировал, чтобы всегда загружались все варианты
//    // TODO: в строчке сразу ниже вместо 1 надо вернуть 5. 1 там было во время тестирования функции
//    if (currentMinute % 1 == 0) {
//        fileWithLinks << baseLink + "300\n";
//        fileWithLinks << baseLink + "900\n";
//        fileWithLinks << baseLink + "1800\n";
//    }
//    if (currentMinute % 15 == 0) {
//        fileWithLinks << baseLink + "3600\n";
//    }
//    if (currentMinute % 30 == 0) {
//        fileWithLinks << baseLink + "18000\n";
//    }
//    // currentMinute == 0 <=> раз в час
//    // TODO: я не запрашиваю сигналы на 7 часов, но в results.bd они есть. Надо разобраться, почему
//    //  и добавить скачивание на 2 дня. И, возможно, на другие ТФ
//    if (currentMinute == 0) {
//        fileWithLinks << baseLink + "86400\n";
//    }

    fileWithLinks.close();

}

void createAndFillFileWithLinks(const char* filepath,
                                         int currentHour,
                                         int currentMinute) {

    // Проверяем, существует ли файл, и если да, то удаляем его.
    checkFileExistsAndDelete(filepath);

    fillFileWithLinks(filepath, currentHour, currentMinute);

    return;
}