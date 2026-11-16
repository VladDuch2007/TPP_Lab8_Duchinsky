
#include "Transactions.h"
#include "Database.h"

#include <cmath>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <sqlite3.h>

namespace {

    using Storage = decltype(createStorage(""));

    void configureStorage(Storage& storage) {
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
    }

}

void runTransactionDemo() {
    auto storage = createStorage("banking_queries.db");
    configureStorage(storage);

    auto accounts = storage.get_all<Account>(
        sqlite_orm::order_by(&Account::id).asc(),
        sqlite_orm::limit(1)
    );

    if (accounts.empty()) {
        throw std::runtime_error("Account not found");
    }

    int accountId = accounts.front().id;

    Account before = storage.get<Account>(accountId);
    int operationsBefore =
        static_cast<int>(storage.count<BankOperation>());

    std::cout << "\n=== COMMIT ===\n";
    std::cout << "Balance before: "
        << before.balance << '\n';
    std::cout << "Operations before: "
        << operationsBefore << '\n';

    bool committed = storage.transaction([&]() {
        Account account = storage.get<Account>(accountId);
        account.balance += 500.0;

        storage.update(account);

        storage.insert(BankOperation{
            0,
            accountId,
            "2026-11-16",
            "deposit",
            500.0,
            "Transaction deposit"
            });

        return true;
        });

    Account afterCommit = storage.get<Account>(accountId);
    int operationsAfterCommit =
        static_cast<int>(storage.count<BankOperation>());

    std::cout << "Committed: "
        << std::boolalpha << committed << '\n';
    std::cout << "Balance after: "
        << afterCommit.balance << '\n';
    std::cout << "Operations after: "
        << operationsAfterCommit << '\n';

    if (!committed ||
        std::abs(afterCommit.balance -
            (before.balance + 500.0)) > 0.001 ||
        operationsAfterCommit != operationsBefore + 1) {
        throw std::runtime_error("COMMIT verification failed");
    }

    std::cout << "\n=== ROLLBACK ===\n";

    double balanceBeforeRollback = afterCommit.balance;
    int operationsBeforeRollback = operationsAfterCommit;

    bool failureDetected = false;

    try {
        storage.transaction([&]() {
            Account account = storage.get<Account>(accountId);
            account.balance += 700.0;

            storage.update(account);

            storage.insert(BankOperation{
                0,
                -1,
                "2026-11-16",
                "deposit",
                700.0,
                "Invalid operation"
                });

            return true;
            });
    }
    catch (const std::exception& ex) {
        failureDetected = true;
        std::cout << "Error: " << ex.what() << '\n';
    }

    Account afterRollback = storage.get<Account>(accountId);
    int operationsAfterRollback =
        static_cast<int>(storage.count<BankOperation>());

    bool rollbackCorrect =
        failureDetected &&
        std::abs(afterRollback.balance -
            balanceBeforeRollback) < 0.001 &&
        operationsAfterRollback == operationsBeforeRollback;

    std::cout << "Balance before: "
        << balanceBeforeRollback << '\n';
    std::cout << "Balance after: "
        << afterRollback.balance << '\n';
    std::cout << "Operations before: "
        << operationsBeforeRollback << '\n';
    std::cout << "Operations after: "
        << operationsAfterRollback << '\n';
    std::cout << "Rollback successful: "
        << rollbackCorrect << '\n';

    if (!rollbackCorrect) {
        throw std::runtime_error("ROLLBACK verification failed");
    }
}

void runWithoutTransactionDemo() {
    const std::string databaseFile =
        "banking_no_transaction.db";

    std::filesystem::remove(databaseFile);

    auto storage = createStorage(databaseFile);
    configureStorage(storage);

    int accountId = storage.insert(Account{
        0,
        "Test Account",
        "TEST-UA-999",
        "UAH",
        1000.0
        });

    Account before = storage.get<Account>(accountId);

    std::cout << "\n=== WITHOUT TRANSACTION ===\n";
    std::cout << "Balance before: "
        << before.balance << '\n';

    bool failureDetected = false;

    try {
        Account account = storage.get<Account>(accountId);
        account.balance += 500.0;

        storage.update(account);

        storage.insert(BankOperation{
            0,
            -1,
            "2026-11-16",
            "deposit",
            500.0,
            "Invalid operation"
            });
    }
    catch (const std::exception& ex) {
        failureDetected = true;
        std::cout << "Error: " << ex.what() << '\n';
    }

    Account after = storage.get<Account>(accountId);

    int operations =
        static_cast<int>(storage.count<BankOperation>());

    bool partialSave =
        failureDetected &&
        std::abs(after.balance -
            (before.balance + 500.0)) < 0.001 &&
        operations == 0;

    std::cout << "Balance after: "
        << after.balance << '\n';
    std::cout << "Operations: "
        << operations << '\n';
    std::cout << "Partial changes saved: "
        << std::boolalpha << partialSave << '\n';

    if (!partialSave) {
        throw std::runtime_error(
            "Non-transaction test failed"
        );
    }
}