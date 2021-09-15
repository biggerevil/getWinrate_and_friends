cmake_minimum_required(VERSION 3.20)
project(checkHistory)

# -Ofast is turned on for all projects (Debug AND Release). Should be only for Release.
set(CMAKE_CXX_FLAGS "-Ofast -Wall -Wextra")

set(CMAKE_CXX_STANDARD 17)

add_executable(checkHistory main.cpp constants.h stakeFromDB.cpp stakeFromDB.h sqlFunctions.cpp sqlFunctions.h)

find_package (SQLite3)
if (SQLITE3_FOUND)
    message("Found sqlite3, and...")
    include_directories(${SQLite3_INCLUDE_DIR})
    target_link_libraries (checkHistory ${SQLite3_LIBRARY})
endif (SQLITE3_FOUND)
