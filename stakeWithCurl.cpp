//
// Created by c on 08.09.2021.
//

#include <iostream>
#include <stdlib.h>
#include <chrono>

#include "stakeWithCurl.h"

int makeStakeWithSystemCommand(StakeOptions newStake) {
    // Get starting timepoint
    auto start = std::chrono::high_resolution_clock::now();

//    TODO: сделать какую-либо функцию для красивого (функционально как минимум) вывода, какую собираешься сделать ставку
    std::cout << "Started making stake in intrade for " << newStake._pairname << ", " << newStake._expireTime << std::endl;
    std::cout << "\n Gonna call this system for making stake" << "\n" << std::endl;
//    std::string curlCommandForSystem = "curl -X \"POST\" \"https://inbar.pro/ajax5.php\" \\\n"
//                                       "     -H 'Host: inbar.pro' \\\n"
//                                       "     -H 'Sec-Ch-Ua: \" Not A;Brand\";v=\"99\", \"Chromium\";v=\"92\"' \\\n"
//                                       "     -H 'Accept: */*' \\\n"
//                                       "     -H 'X-Requested-With: XMLHttpRequest' \\\n"
//                                       "     -H 'Sec-Ch-Ua-Mobile: ?0' \\\n"
//                                       "     -H 'User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/92.0.4515.159 Safari/537.36' \\\n"
//                                       "     -H 'Content-Type: application/x-www-form-urlencoded' \\\n"
//                                       "     -H 'Origin: https://inbar.pro' \\\n"
//                                       "     -H 'Sec-Fetch-Site: same-origin' \\\n"
//                                       "     -H 'Sec-Fetch-Mode: cors' \\\n"
//                                       "     -H 'Sec-Fetch-Dest: empty' \\\n"
//                                       "     -H 'Referer: https://inbar.pro/' \\\n"
//                                       "     -H 'Accept-Encoding: gzip, deflate' \\\n"
//                                       "     -H 'Accept-Language: ru-RU,ru;q=0.9,en-US;q=0.8,en;q=0.7' \\\n"
//                                       "     -H 'Cookie: __ddg1=1C2JHOKeE8CcrhkSHy7J' \\\n"
//                                       "     --data-urlencode \"user_id=82168\" \\\n"
//                                       "     --data-urlencode \"user_hash=b961382808ae6163f2f0af2248d72ea8\" \\\n"
//                                       "     --data-urlencode \"option=" + newStake._pairname + "\" \\\n"
//                                       "     --data-urlencode \"investment=" + newStake._stakeSum + "\" \\\n"
//                                       "     --data-urlencode \"time=" + newStake._expireTime + "\" \\\n"
//                                       "     --data-urlencode \"date=0\" \\\n"
//                                       "     --data-urlencode \"trade_type=sprint\" \\\n"
//                                       "     --data-urlencode \"status=" + (newStake._willGoHigher ? "1" : "2") +  "\"";

//    std::string curlCommandForSystem = "curl -i -s -k -X $'POST' \\\n"
//                                       "    -H $'Host: inbar.pro' -H $'Sec-Ch-Ua: \\\" Not A;Brand\\\";v=\\\"99\\\", \\\"Chromium\\\";v=\\\"92\\\"' -H $'Accept: */*' -H $'X-Requested-With: XMLHttpRequest' -H $'Sec-Ch-Ua-Mobile: ?0' -H $'User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/92.0.4515.159 Safari/537.36' -H $'Content-Type: application/x-www-form-urlencoded' -H $'Origin: https://inbar.pro' -H $'Sec-Fetch-Site: same-origin' -H $'Sec-Fetch-Mode: cors' -H $'Sec-Fetch-Dest: empty' -H $'Referer: https://inbar.pro/' -H $'Accept-Encoding: gzip, deflate' -H $'Accept-Language: ru-RU,ru;q=0.9,en-US;q=0.8,en;q=0.7' \\\n"
//                                       "    -b $'__ddg1=9Euon8h00PQBUIlwMeIf; __ddgid=lYnka2XJshGSoua9; _ym_uid=1631044950410451281; _ym_d=1631044950; supportOnlineTalkID=OlhoqJXsTQKPp64yW8Pd2a1mMVzLdkfX; user_id=82168; user_hash=b961382808ae6163f2f0af2248d72ea8; _ga=GA1.2.655170758.1631044967; __ddgmark=jnJmFHGx1Ez6iw64; one_trade=1; currency_name=RUB; _gid=GA1.2.177490857.1631689026; __ddg2=EjGYKQp5esiSSBBj; _gat_gtag_UA_133478727_1=1; option_type=Sprint' \\\n"
//                                       "    --data-binary $'user_id=82168&user_hash=b961382808ae6163f2f0af2248d72ea8&option=" + newStake._pairname + "&investment=" + newStake._stakeSum + "&time=" + newStake._expireTime + "&date=0&trade_type=sprint&status=" + (newStake._willGoHigher ? "1" : "2") + "' \\\n"
//                                       "    $'https://inbar.pro/ajax5.php' >> output.txt";

    std::string httpieCommandForSystem = "http --ignore-stdin --form POST 'https://inbar.pro/ajax5.php' \\\n"
                                         "    'Host':'inbar.pro' \\\n"
                                         "    'Sec-Ch-Ua':'\\\" Not A;Brand\\\";v=\\\"99\\\", \\\"Chromium\\\";v=\\\"92\\\"' \\\n"
                                         "    'Accept':'*/*' \\\n"
                                         "    'X-Requested-With':'XMLHttpRequest' \\\n"
                                         "    'Sec-Ch-Ua-Mobile':'?0' \\\n"
                                         "    'User-Agent':'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/92.0.4515.159 Safari/537.36' \\\n"
                                         "    'Content-Type':'application/x-www-form-urlencoded' \\\n"
                                         "    'Origin':'https://inbar.pro' \\\n"
                                         "    'Sec-Fetch-Site':'same-origin' \\\n"
                                         "    'Sec-Fetch-Mode':'cors' \\\n"
                                         "    'Sec-Fetch-Dest':'empty' \\\n"
                                         "    'Referer':'https://inbar.pro/' \\\n"
                                         "    'Accept-Encoding':'gzip, deflate' \\\n"
                                         "    'Accept-Language':'ru-RU,ru;q=0.9,en-US;q=0.8,en;q=0.7' \\\n"
                                         "    'Cookie':'__ddg1=9Euon8h00PQBUIlwMeIf; __ddgid=lYnka2XJshGSoua9; _ym_uid=1631044950410451281; _ym_d=1631044950; supportOnlineTalkID=OlhoqJXsTQKPp64yW8Pd2a1mMVzLdkfX; user_id=82168; user_hash=b961382808ae6163f2f0af2248d72ea8; _ga=GA1.2.655170758.1631044967; __ddgmark=jnJmFHGx1Ez6iw64; one_trade=1; currency_name=RUB; _gid=GA1.2.177490857.1631689026; __ddg2=EjGYKQp5esiSSBBj; _gat_gtag_UA_133478727_1=1; option_type=Sprint' \\\n"
                                         "    'user_id'='82168' \\\n"
                                         "    'user_hash'='b961382808ae6163f2f0af2248d72ea8' \\\n"
                                         "    'option'='" + newStake._pairname + "' \\\n"
                                         "    'investment'='" + newStake._stakeSum + "' \\\n"
                                         "    'time'='" + newStake._expireTime + "' \\\n"
                                         "    'date'='0' \\\n"
                                         "    'trade_type'='sprint' \\\n"
                                         "    'status'='" + (newStake._willGoHigher ? "1" : "2") + "'\n";

//    std::string httpieCommandForSystem = "http --ignore-stdin --form POST 'https://inbar.pro/ajax5.php' \\\n"
//                                         "    'Host':'inbar.pro' \\\n"
//                                         "    'Sec-Ch-Ua':'\\\" Not A;Brand\\\";v=\\\"99\\\", \\\"Chromium\\\";v=\\\"92\\\"' \\\n"
//                                         "    'Accept':'*/*' \\\n"
//                                         "    'X-Requested-With':'XMLHttpRequest' \\\n"
//                                         "    'Sec-Ch-Ua-Mobile':'?0' \\\n"
//                                         "    'User-Agent':'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/92.0.4515.159 Safari/537.36' \\\n"
//                                         "    'Content-Type':'application/x-www-form-urlencoded' \\\n"
//                                         "    'Origin':'https://inbar.pro' \\\n"
//                                         "    'Sec-Fetch-Site':'same-origin' \\\n"
//                                         "    'Sec-Fetch-Mode':'cors' \\\n"
//                                         "    'Sec-Fetch-Dest':'empty' \\\n"
//                                         "    'Referer':'https://inbar.pro/' \\\n"
//                                         "    'Accept-Encoding':'gzip, deflate' \\\n"
//                                         "    'Accept-Language':'ru-RU,ru;q=0.9,en-US;q=0.8,en;q=0.7' \\\n"
//                                         "    'user_id'='82168' \\\n"
//                                         "    'user_hash'='b961382808ae6163f2f0af2248d72ea8' \\\n"
//                                         "    'option'='" + newStake._pairname + "' \\\n"
//                                         "    'investment'='" + newStake._stakeSum + "' \\\n"
//                                         "    'time'='" + newStake._expireTime + "' \\\n"
//                                         "    'date'='0' \\\n"
//                                         "    'trade_type'='sprint' \\\n"
//                                         "    'status'='" + (newStake._willGoHigher ? "1" : "2") + "'\\\n";

//    std::string httpieCommandForSystem = "http --ignore-stdin --form POST 'https://inbar.pro/ajax5.php' \\\n"
//                                         "    'Host':'inbar.pro' \\\n"
//                                         "    'Sec-Ch-Ua':'\\\" Not A;Brand\\\";v=\\\"99\\\", \\\"Chromium\\\";v=\\\"92\\\"' \\\n"
//                                         "    'Accept':'*/*' \\\n"
//                                         "    'X-Requested-With':'XMLHttpRequest' \\\n"
//                                         "    'Sec-Ch-Ua-Mobile':'?0' \\\n"
//                                         "    'User-Agent':'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/92.0.4515.159 Safari/537.36' \\\n"
//                                         "    'Content-Type':'application/x-www-form-urlencoded' \\\n"
//                                         "    'Origin':'https://inbar.pro' \\\n"
//                                         "    'Sec-Fetch-Site':'same-origin' \\\n"
//                                         "    'Sec-Fetch-Mode':'cors' \\\n"
//                                         "    'Sec-Fetch-Dest':'empty' \\\n"
//                                         "    'Referer':'https://inbar.pro/' \\\n"
//                                         "    'Accept-Encoding':'gzip, deflate' \\\n"
//                                         "    'Accept-Language':'ru-RU,ru;q=0.9,en-US;q=0.8,en;q=0.7' \\\n"
//                                         "    'user_id'='82168' \\\n"
//                                         "    'user_hash'='b961382808ae6163f2f0af2248d72ea8' \\\n"
//                                         "    'option'='" + newStake._pairname + "' \\\n"
//                                          "    'investment'='" + newStake._stakeSum + "' \\\n"
//                                          "    'time'='" + newStake._expireTime + "' \\\n"
//                                          "    'date'='0' \\\n"
//                                          "    'trade_type'='sprint' \\\n"
//                                          "    'status'='" + (newStake._willGoHigher ? "1" : "2") + "' >> output.txt\\\n";



//    std::string callCppProgrammForMakingStake = "/root/executable";


//    std::string makeStakeWithHttPie = "http --ignore-stdin --form POST 'https://inbar.pro/ajax5.php' 'Host':'inbar.pro' 'Sec-Ch-Ua':'\\\" Not A;Brand\\\";v=\\\"99\\\", \\\"Chromium\\\";v=\\\"92\\\"' 'Accept':'*/*' 'X-Requested-With':'XMLHttpRequest' 'Sec-Ch-Ua-Mobile':'?0' 'User-Agent':'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/92.0.4515.159 Safari/537.36' 'Content-Type':'application/x-www-form-urlencoded' \\ 'Origin':'https://inbar.pro' \\ 'Sec-Fetch-Site':'same-origin' \\ 'Sec-Fetch-Mode':'cors' \\ 'Sec-Fetch-Dest':'empty' \\ 'Referer':'https://inbar.pro/' \\ 'Accept-Encoding':'gzip, deflate' \\ 'Accept-Language':'ru-RU,ru;q=0.9,en-US;q=0.8,en;q=0.7' \\ 'user_id'='82168' \\ 'user_hash'='b961382808ae6163f2f0af2248d72ea8' \\ 'option'='EURUSD' \\ 'investment'='50' \\ 'time'='3' \\ 'date'='0' \\ 'trade_type'='sprint'";

//    std::string makeStakeWithHttPie = "http --ignore-stdin --form POST 'https://inbar.pro/ajax5.php' 'Host':'inbar.pro' \
//    'Sec-Ch-Ua':'\" Not A;Brand\";v=\"99\", \"Chromium\";v=\"92\"' \
//    'Accept':'*/*' \
//    'X-Requested-With':'XMLHttpRequest' \
//    'Sec-Ch-Ua-Mobile':'?0' \
//    'User-Agent':'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/92.0.4515.159 Safari/537.36' \
//    'Content-Type':'application/x-www-form-urlencoded' \
//    'Origin':'https://inbar.pro' \
//    'Sec-Fetch-Site':'same-origin' \
//    'Sec-Fetch-Mode':'cors' \
//    'Sec-Fetch-Dest':'empty' \
//    'Referer':'https://inbar.pro/' \
//    'Accept-Encoding':'gzip, deflate' \
//    'Accept-Language':'ru-RU,ru;q=0.9,en-US;q=0.8,en;q=0.7' \
//    'user_id'='82168' \
//    'user_hash'='b961382808ae6163f2f0af2248d72ea8' \
//    'option'='" + newStake._pairname + "' \
//    'investment'='" + newStake._stakeSum + "' \
//    'time'='" + newStake._expireTime + "' \
//    'date'='0' \
//    'trade_type'='sprint' \
//    'status'='" + (newStake._willGoHigher ? "1" : "2") + "'";


    std::cout << "Gonna run this command:\n" << httpieCommandForSystem << std::endl;
    std::cout << "whoami = " << system("whoami") << std::endl;

    system(httpieCommandForSystem.c_str());
    std::cout << "Done with calling system command for making stake" << std::endl;

    // Get ending timepoint
    auto stop = std::chrono::high_resolution_clock::now();

    // Get duration. Substart timepoints to
    // get durarion. To cast it to proper unit
    // use duration cast method
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

    std::cout << "Time taken by function makeStakeWithSystemCommand (or at lest it should be this whole function): "
              << duration.count() << " milliseconds" << std::endl;

    return 0;
}