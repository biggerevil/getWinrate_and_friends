cmake_minimum_required(VERSION 3.16)
project(getWinrate)

# -Ofast is turned on for all projects (Debug AND Release). Should be only for Release.
set(CMAKE_CXX_FLAGS "-Ofast -Wall -Wextra")

#if(HOSTNAME STREQUAL "trottling")
#    set(Boost_INCLUDE_DIR "/root/boost_1_74_0")
#endif()

set(CMAKE_CXX_STANDARD 17)
#set(Boost_USE_MULTITHREADED ON)

find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
#find_package(Boost COMPONENTS system REQUIRED)
find_package(CURL)
find_package (Python COMPONENTS Interpreter Development)
find_package(PythonLibs REQUIRED)
#include_directories(/usr/local/include ${OPENSSL_INCLUDE_DIR} ${Boost_INCLUDE_DIR} ${PYTHON_INCLUDE_DIRS})
# WIthout Boost:
include_directories(/usr/local/include ${OPENSSL_INCLUDE_DIR} ${PYTHON_INCLUDE_DIRS})
if (CURL_FOUND)
    include_directories(${CURL_INCLUDE_DIRS})
    add_definitions(-DHAVE_CURL)
endif()

#add_executable(getWinrate main.cpp constant.h createFileWithLinks.cpp createFileWithLinks.h checkOpenStakes.cpp checkOpenStakes.h downloadFiles.cpp downloadFiles.h getResultsOfOpenStakes.cpp getResultsOfOpenStakes.h huntForSignals.cpp huntForSignals.h pythonBinaryApi.cpp pythonBinaryApi.h beastWebSocket.cpp beastWebSocket.h root_certificates.hpp cleanDb.cpp cleanDb.h rubyPart.cpp rubyPart.h)
# Without Boost amd websic==ocket:
add_executable(getWinrate main.cpp constant.h createFileWithLinks.cpp createFileWithLinks.h checkOpenStakes.cpp checkOpenStakes.h downloadFiles.cpp downloadFiles.h getResultsOfOpenStakes.cpp getResultsOfOpenStakes.h huntForSignals.cpp huntForSignals.h pythonBinaryApi.cpp pythonBinaryApi.h root_certificates.hpp cleanDb.cpp cleanDb.h rubyPart.cpp rubyPart.h stakeWithCurl.cpp stakeWithCurl.h credentials.h)

find_package (SQLite3)
if (SQLITE3_FOUND)
    message("Found sqlite3, and...")
    include_directories(${SQLite3_INCLUDE_DIR})
    target_link_libraries (getWinrate ${SQLite3_LIBRARY})
endif (SQLITE3_FOUND)

#target_link_libraries(getWinrate ${CMAKE_THREAD_LIBS_INIT}
#        ${OPENSSL_LIBRARIES} ${Boost_LIBRARIES} ${CURL_LIBRARIES} ${PYTHON_LIBRARIES})
# Without Boost:
target_link_libraries(getWinrate ${CMAKE_THREAD_LIBS_INIT}
        ${OPENSSL_LIBRARIES} ${CURL_LIBRARIES} ${PYTHON_LIBRARIES})

# Версия с libTgBot
#target_link_libraries(getWinrate /usr/local/lib/libTgBot.a ${CMAKE_THREAD_LIBS_INIT}
#${OPENSSL_LIBRARIES} ${Boost_LIBRARIES} ${CURL_LIBRARIES})