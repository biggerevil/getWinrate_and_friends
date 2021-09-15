cmake_minimum_required(VERSION 3.16)
project(collectStats)

#set(CMAKE_BUILD_TYPE Release)
#set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(CMAKE_CXX_STANDARD 17)

add_executable(collectStats main.cpp copyDatabase.cpp copyDatabase.h someConstants.h commandsForDb.cpp commandsForDb.h daysToCheck.cpp daysToCheck.h)

PYTHON_INCLUDE_DIR=/Library/Frameworks/Python.framework/Versions/3.7/bin/python3
PYTHON_LIBRARY=

find_package(Python3 COMPONENTS Development NumPy)
target_include_directories(collectStats PRIVATE ${Python3_INCLUDE_DIRS}  ${Python3_NumPy_INCLUDE_DIRS})
target_link_libraries (collectStats ${SQLite3_LIBRARY} Python3::Python Python3::NumPy)

find_package (SQLite3)
if (SQLITE3_FOUND)
    message("Found sqlite3, and...")
    include_directories(${SQLite3_INCLUDE_DIR})
endif (SQLITE3_FOUND)