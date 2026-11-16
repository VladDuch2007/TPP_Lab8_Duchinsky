
# Laboratory Work No. 8 - TPP

## Student

Duchinsky Vladyslav Stanislavovych

Group: FIT-3-17

## Topic

Working with Databases Using ORM

## Project

Banking Operations Management System

## Technologies

- C++20
- Microsoft Visual Studio
- CMake
- vcpkg
- SQLite3
- sqlite_orm

## Description

The application manages bank accounts and
their associated banking operations.

The database contains two related tables:

- Accounts
- BankOperations

The relationship is one-to-many (1:N).

## Features

- Database creation using SQLite
- Object-relational mapping
- Repository design pattern
- CRUD operations
- Filtering and sorting
- LIMIT queries
- COUNT, SUM and AVG
- GROUP BY
- INNER JOIN
- Transactions with COMMIT and ROLLBACK
- Comparison of operations with and without transactions
- Comparison of SQLite C API and sqlite_orm
- Database error handling

## Project Structure

include/
- Account.h
- BankOperation.h
- Database.h
- IAccountRepository.h
- SQLiteAccountRepository.h
- RepositoryDemo.h
- TestData.h
- Queries.h
- Transactions.h
- Comparison.h

src/
- main.cpp
- SQLiteAccountRepository.cpp
- RepositoryDemo.cpp
- TestData.cpp
- Queries.cpp
- Transactions.cpp
- Comparison.cpp

Configuration:
- CMakeLists.txt
- CMakePresets.json
- vcpkg.json

## Building and Running

1. Install Visual Studio with C++ development tools.
2. Ensure that CMake and vcpkg are available.
3. Open the project folder in Visual Studio.
4. Select the default CMake preset.
5. Configure the project.
6. Build using Build All.
7. Select Lab8_Banking.exe.
8. Run using Ctrl+F5.

The required SQLite libraries are installed
through vcpkg using the project manifest.

## Test Data

The application prepares:

- 5 bank accounts
- 15 banking operations

Test results:

- Total operation volume: 99550.00
- Average operation amount: 6636.67

## Transaction Results

COMMIT:
- Balance changed from 15000 to 15500
- Operation count changed from 15 to 16

ROLLBACK:
- Balance remained 15500
- Operation count remained 16

Without transaction:
- Balance changed from 1000 to 1500
- Related operation was not saved

## SQLite C API vs ORM

Both approaches successfully inserted
and retrieved equivalent account records.

The comparison test returned:

Results match: true

## Conclusion

The application demonstrates database
management using SQLite and sqlite_orm.

The Repository pattern separates database
access from application logic.

Transactions ensure atomic execution
of related banking operations.