
#include "RepositoryDemo.h"
#include "IAccountRepository.h"
#include "SQLiteAccountRepository.h"

#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>

void runRepositoryDemo() {
    const std::string databaseFile =
        "banking_repository_demo.db";

    std::filesystem::remove(databaseFile);

    SQLiteAccountRepository sqliteRepository(databaseFile);
    IAccountRepository& repository = sqliteRepository;

    std::cout << "\n=== REPOSITORY CRUD ===\n";

    Account account{
        0,
        "Vladyslav Duchinsky",
        "TEST-UA-001",
        "UAH",
        15000.0
    };

    int id = repository.add(account);

    std::cout << "\nCREATE\n";
    std::cout << "Account ID: " << id << '\n';

    auto found = repository.findById(id);

    if (!found) {
        throw std::runtime_error("READ failed");
    }

    std::cout << "\nREAD\n";
    std::cout << "Owner: " << found->owner << '\n';
    std::cout << "Number: " << found->accountNumber << '\n';
    std::cout << "Balance: " << found->balance << '\n';

    found->balance = 20000.0;

    bool updated = repository.update(*found);

    auto afterUpdate = repository.findById(id);

    if (!updated || !afterUpdate ||
        afterUpdate->balance != 20000.0) {
        throw std::runtime_error("UPDATE failed");
    }

    std::cout << "\nUPDATE\n";
    std::cout << "Updated: " << std::boolalpha
        << updated << '\n';
    std::cout << "New balance: "
        << afterUpdate->balance << '\n';

    std::cout << "\nREAD ALL\n";

    auto accounts = repository.findAll();

    for (const auto& item : accounts) {
        std::cout
            << item.id << " | "
            << item.owner << " | "
            << item.accountNumber << " | "
            << item.balance << '\n';
    }

    bool removed = repository.remove(id);
    auto afterDelete = repository.findById(id);

    if (!removed || afterDelete.has_value()) {
        throw std::runtime_error("DELETE failed");
    }

    std::cout << "\nDELETE\n";
    std::cout << "Removed: " << removed << '\n';
    std::cout << "Account exists after deletion: "
        << afterDelete.has_value() << '\n';
}