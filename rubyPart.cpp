//
// Created by c on 05.09.2021.
//

#include <stdlib.h>
#include <iostream>

#include "rubyPart.h"
#include "constant.h"

// Используемый брокер не работает с полуночи до 4 часов утра,
// поэтому здесь проверяю текущее время, и если оно попадает
// в этот промежуток, то прекращаю исполнение функции
bool checkIfShouldExit() {
    int currentHour = std::stoi(findCurrentTime("%H"));
    if (currentHour >= 0 and currentHour <= 4) {
        return true;
    }
    return false;
}

void make_stake_with_ruby(bool willGoHigher) {

    if (checkIfShouldExit() == true) {
        std::cout << "Похоже, текущее время от полуночи до 4 утра. Выхожу и не делаю ставку." << std::endl;
        return;
    }

    std::cout << "Gonna make stake with Ruby" << std::endl;
    std::string willGoHigherInString = willGoHigher ? "true" : "false";
//    TODO: потенциально тяжело находимая ошибка, не стоит использовать Ruby по такому полному пути, могут быть проблемы
//      впоследствии с версиями, которые не будут отображаться в других вариантах (например, будет немного иная
//      работа)
    std::string commandForRuby = "/usr/local/rvm/rubies/ruby-3.0.0/bin/ruby ~/clicker/main.rb -p USD/CAD -t 120 -s 5000 -w " + willGoHigherInString + " > ~/logs_of_clicker/logs_for_" + findCurrentTime("%Y-%m-%d_%H_%M") + ".txt 2>&1";
    std::cout << "commandForRuby = " << commandForRuby << std::endl;
    std::system("setenv(GEM_HOME, /usr/local/rvm/gems/ruby-3.0.0)");
//    Path + added path to geckodriver in the end
    setenv("PATH", "/usr/local/rvm/gems/ruby-3.0.0/bin:/usr/local/rvm/gems/ruby-3.0.0@global/bin:/usr/local/rvm/rubies/ruby-3.0.0/bin:/usr/local/rvm/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin:/root/geckodriver_for_global_gemset", 0);
    setenv("GEM_HOME", "/usr/local/rvm/gems/ruby-3.0.0", 0);
    setenv("GEM_PATH", "/usr/local/rvm/gems/ruby-3.0.0:/usr/local/rvm/gems/ruby-3.0.0@global", 0);
    std::system("cd ~/clicker");
    std::system("rvm use 3.0.0");
    std::system("rvm gemset use global");
    std::system("whoami");
    std::system("env");
    std::system(commandForRuby.c_str());
    std::system("cd -");
    std::cout << "Finished calling ruby script!" << std::endl;
}