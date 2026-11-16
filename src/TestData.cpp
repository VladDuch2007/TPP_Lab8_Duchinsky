
#include "TestData.h"
#include "Database.h"

#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <sqlite3.h>

void prepareTestData() {
    const std::string databaseFile = "banking_queries.db";

    std::filesystem::remove(databaseFile);

    auto storage = createStorage(databaseFile);

    storage.on_open = [](sqlite3* db) {
        if (sqlite3_exec(
            db,
            "PRAGMA foreign_keys = ON;",
            nullptr,
            nullptr,
            nullptr
        ) != SQLITE_OK) {
            throw std::runtime_error(
                "Cannot enable foreign keys"
            );
        }
        };

    storage.open_forever();
    storage.sync_schema();

    std::vector<Account> accounts = {
        { 0, "Vladyslav Duchinsky", "TEST-UA-001", "UAH", 15000.0 },
        { 0, "Olena Kovalenko", "TEST-UA-002", "UAH", 8200.0 },
        { 0, "Andrii Melnyk", "TEST-UA-003", "UAH", 26000.0 },
        { 0, "Iryna Shevchenko", "TEST-UA-004", "UAH", 7250.0 },
        { 0, "Maksym Bondarenko", "TEST-UA-005", "UAH", 17500.0 }
    };

    std::vector<int> accountIds;

    for (const auto& account : accounts) {
        accountIds.push_back(storage.insert(account));
    }

    std::vector<BankOperation> operations = {
        { 0, accountIds[0], "2026-11-01", "deposit", 12000.0, "Initial deposit" },
        { 0, accountIds[0], "2026-11-03", "withdrawal", 2000.0, "Cash withdrawal" },
        { 0, accountIds[0], "2026-11-05", "deposit", 5000.0, "Salary" },

        { 0, accountIds[1], "2026-11-01", "deposit", 9000.0, "Initial deposit" },
        { 0, accountIds[1], "2026-11-04", "deposit", 1500.0, "Transfer received" },
        { 0, accountIds[1], "2026-11-07", "withdrawal", 2300.0, "Purchase" },

        { 0, accountIds[2], "2026-11-02", "deposit", 25000.0, "Initial deposit" },
        { 0, accountIds[2], "2026-11-06", "withdrawal", 4000.0, "Payment" },
        { 0, accountIds[2], "2026-11-09", "deposit", 5000.0, "Transfer received" },

        { 0, accountIds[3], "2026-11-02", "deposit", 7000.0, "Initial deposit" },
        { 0, accountIds[3], "2026-11-08", "withdrawal", 1000.0, "Cash withdrawal" },
        { 0, accountIds[3], "2026-11-11", "deposit", 1250.0, "Refund" },

        { 0, accountIds[4], "2026-11-03", "deposit", 19000.0, "Initial deposit" },
        { 0, accountIds[4], "2026-11-10", "withdrawal", 3500.0, "Payment" },
        { 0, accountIds[4], "2026-11-12", "deposit", 2000.0, "Transfer received" }
    };

    for (const auto& operation : operations) {
        storage.insert(operation);
    }

    std::cout << "Test database prepared\n";
    std::cout << "Accounts: "
        << storage.count<Account>() << '\n';
    std::cout << "Bank operations: "
        << storage.count<BankOperation>() << '\n';
}