
cmake_minimum_required(VERSION 3.21)

project(Lab8_Banking LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SqliteOrm CONFIG REQUIRED)
find_package(unofficial-sqlite3 CONFIG REQUIRED)

add_executable(Lab8_Banking
    src/main.cpp
    src/SQLiteAccountRepository.cpp
    src/RepositoryDemo.cpp
    src/TestData.cpp
    src/Queries.cpp
    src/Transactions.cpp
    src/Comparison.cpp
)

target_include_directories(Lab8_Banking PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(Lab8_Banking PRIVATE
    sqlite_orm::sqlite_orm
    unofficial::sqlite3::sqlite3
)