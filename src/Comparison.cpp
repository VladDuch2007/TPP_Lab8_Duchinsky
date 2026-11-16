
#include "Comparison.h"
#include "Database.h"

#include <sqlite3.h>
#include <cmath>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

namespace {

    using DatabaseHandle =
        std::unique_ptr<sqlite3, decltype(&sqlite3_close)>;

    using StatementHandle =
        std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)>;

    void checkResult(int result, sqlite3* db) {
        if (result != SQLITE_OK) {
            throw std::runtime_error(sqlite3_errmsg(db));
        }
    }

    StatementHandle prepareStatement(
        sqlite3* db,
        const char* sql
    ) {
        sqlite3_stmt* rawStatement = nullptr;

        if (sqlite3_prepare_v2(
            db, sql, -1, &rawStatement, nullptr
        ) != SQLITE_OK) {
            std::string error = sqlite3_errmsg(db);
            sqlite3_finalize(rawStatement);
            throw std::runtime_error(error);
        }

        return StatementHandle(
            rawStatement,
            &sqlite3_finalize
        );
    }

    std::string getText(sqlite3_stmt* statement, int column) {
        const unsigned char* value =
            sqlite3_column_text(statement, column);

        if (!value) {
            throw std::runtime_error("Invalid text value");
        }

        return reinterpret_cast<const char*>(value);
    }

    Account runSQLiteCApi() {
        const std::string filename =
            "banking_raw_comparison.db";

        std::filesystem::remove(filename);

        sqlite3* rawDatabase = nullptr;
        int result = sqlite3_open(
            filename.c_str(),
            &rawDatabase
        );

        DatabaseHandle db(rawDatabase, &sqlite3_close);

        if (result != SQLITE_OK) {
            throw std::runtime_error(
                "Cannot open raw SQLite database"
            );
        }

        const char* createSql =
            "CREATE TABLE Accounts ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "owner TEXT NOT NULL,"
            "accountNumber TEXT NOT NULL,"
            "currency TEXT NOT NULL,"
            "balance REAL NOT NULL"
            ");";

        char* errorMessage = nullptr;

        result = sqlite3_exec(
            db.get(),
            createSql,
            nullptr,
            nullptr,
            &errorMessage
        );

        if (result != SQLITE_OK) {
            std::string message = errorMessage
                ? errorMessage
                : sqlite3_errmsg(db.get());

            sqlite3_free(errorMessage);
            throw std::runtime_error(message);
        }

        int accountId = 0;

        {
            const char* insertSql =
                "INSERT INTO Accounts "
                "(owner, accountNumber, currency, balance) "
                "VALUES (?, ?, ?, ?);";

            auto statement = prepareStatement(
                db.get(),
                insertSql
            );

            checkResult(
                sqlite3_bind_text(
                    statement.get(), 1,
                    "Test User", -1, SQLITE_TRANSIENT
                ),
                db.get()
            );

            checkResult(
                sqlite3_bind_text(
                    statement.get(), 2,
                    "TEST-UA-888", -1, SQLITE_TRANSIENT
                ),
                db.get()
            );

            checkResult(
                sqlite3_bind_text(
                    statement.get(), 3,
                    "UAH", -1, SQLITE_TRANSIENT
                ),
                db.get()
            );

            checkResult(
                sqlite3_bind_double(
                    statement.get(), 4, 12345.67
                ),
                db.get()
            );

            if (sqlite3_step(statement.get()) != SQLITE_DONE) {
                throw std::runtime_error(
                    sqlite3_errmsg(db.get())
                );
            }

            accountId = static_cast<int>(
                sqlite3_last_insert_rowid(db.get())
                );
        }

        const char* selectSql =
            "SELECT id, owner, accountNumber, "
            "currency, balance "
            "FROM Accounts WHERE id = ?;";

        auto statement = prepareStatement(
            db.get(),
            selectSql
        );

        checkResult(
            sqlite3_bind_int(
                statement.get(), 1, accountId
            ),
            db.get()
        );

        if (sqlite3_step(statement.get()) != SQLITE_ROW) {
            throw std::runtime_error(
                "Account not found using SQLite C API"
            );
        }

        Account account{
            sqlite3_column_int(statement.get(), 0),
            getText(statement.get(), 1),
            getText(statement.get(), 2),
            getText(statement.get(), 3),
            sqlite3_column_double(statement.get(), 4)
        };

        return account;
    }

    Account runSQLiteORM() {
        const std::string filename =
            "banking_orm_comparison.db";

        std::filesystem::remove(filename);

        auto storage = createStorage(filename);
        storage.sync_schema();

        Account account{
            0,
            "Test User",
            "TEST-UA-888",
            "UAH",
            12345.67
        };

        int accountId = storage.insert(account);

        auto result = storage.get_optional<Account>(accountId);

        if (!result) {
            throw std::runtime_error(
                "Account not found using ORM"
            );
        }

        return *result;
    }

}

void runComparisonDemo() {
    Account rawResult = runSQLiteCApi();
    Account ormResult = runSQLiteORM();

    std::cout << std::fixed << std::setprecision(2);

    std::cout << "\n=== SQLITE C API VS ORM ===\n";

    std::cout << "SQLite C API:\n";
    std::cout << rawResult.id << " | "
        << rawResult.owner << " | "
        << rawResult.accountNumber << " | "
        << rawResult.balance << '\n';

    std::cout << "sqlite_orm:\n";
    std::cout << ormResult.id << " | "
        << ormResult.owner << " | "
        << ormResult.accountNumber << " | "
        << ormResult.balance << '\n';

    bool identical =
        rawResult.id == ormResult.id &&
        rawResult.owner == ormResult.owner &&
        rawResult.accountNumber == ormResult.accountNumber &&
        rawResult.currency == ormResult.currency &&
        std::abs(
            rawResult.balance - ormResult.balance
        ) < 0.000001;

    std::cout << "Results match: "
        << std::boolalpha << identical << '\n';

    if (!identical) {
        throw std::runtime_error(
            "SQLite C API and ORM results differ"
        );
    }
}