
#include "Queries.h"
#include "Database.h"

#include <iostream>
#include <iomanip>

void runQueryDemo() {
    using namespace sqlite_orm;

    auto storage = createStorage("banking_queries.db");
    storage.sync_schema();

    std::cout << std::fixed << std::setprecision(2);

    std::cout << "\n=== FILTERING ===\n";

    auto richAccounts = storage.get_all<Account>(
        where(c(&Account::balance) >= 10000.0)
    );

    for (const auto& account : richAccounts) {
        std::cout
            << account.id << " | "
            << account.owner << " | "
            << account.balance << '\n';
    }

    std::cout << "\n=== SORTING ===\n";

    auto sortedAccounts = storage.get_all<Account>(
        order_by(&Account::owner).asc()
    );

    for (const auto& account : sortedAccounts) {
        std::cout
            << account.id << " | "
            << account.owner << " | "
            << account.balance << '\n';
    }

    std::cout << "\n=== TOP 3 OPERATIONS ===\n";

    auto topOperations = storage.get_all<BankOperation>(
        order_by(&BankOperation::amount).desc(),
        limit(3)
    );

    for (const auto& operation : topOperations) {
        std::cout
            << operation.id << " | "
            << operation.operationType << " | "
            << operation.amount << '\n';
    }

    std::cout << "\n=== AGGREGATION ===\n";

    auto operationCount = storage.count<BankOperation>();
    auto totalAmount = storage.sum(&BankOperation::amount);
    auto averageAmount = storage.avg(&BankOperation::amount);

    std::cout << "Operation count: "
        << operationCount << '\n';

    std::cout << "Total operation volume: "
        << (totalAmount ? *totalAmount : 0.0) << '\n';

    std::cout << "Average operation amount: "
        << averageAmount << '\n';

    std::cout << "\n=== GROUP BY ACCOUNT ===\n";

    auto grouped = storage.select(
        columns(
            &BankOperation::accountId,
            sum(&BankOperation::amount)
        ),
        group_by(&BankOperation::accountId)
    );

    for (const auto& [accountId, amount] : grouped) {
        std::cout
            << "Account ID: " << accountId
            << " | Total volume: "
            << (amount ? *amount : 0.0) << '\n';
    }

    std::cout << "\n=== INNER JOIN ===\n";

    auto joined = storage.select(
        columns(
            &Account::owner,
            &Account::accountNumber,
            &BankOperation::operationDate,
            &BankOperation::operationType,
            &BankOperation::amount
        ),
        inner_join<BankOperation>(
            on(c(&Account::id) == &BankOperation::accountId)
        ),
        order_by(&BankOperation::id)
    );

    for (const auto& [owner, number, date, type, amount] : joined) {
        std::cout
            << owner << " | "
            << number << " | "
            << date << " | "
            << type << " | "
            << amount << '\n';
    }
}