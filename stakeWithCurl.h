//
// Created by c on 08.09.2021.
//

#ifndef GETWINRATE_STAKEWITHCURL_H
#define GETWINRATE_STAKEWITHCURL_H

struct StakeOptions {
//    Добавляю _ к названиям полей, чтобы при случае можно было легко отличить поля структуры
//    от других переменных с такими же названиями.
    std::string _pairname;
    std::string _expireTime;
    std::string _stakeSum;
    bool _willGoHigher;

    StakeOptions(std::string pairname, std::string expireTime, std::string stakeSum, bool willGoHigher) {
        _pairname = pairname;
        _expireTime = expireTime;
        _stakeSum = stakeSum;
        _willGoHigher = willGoHigher;
    }
};

int makeStakeWithSystemCommand(StakeOptions newStake);

#endif //GETWINRATE_STAKEWITHCURL_H
