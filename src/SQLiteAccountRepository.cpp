#include "SQLiteAccountRepository.h"

#include <stdexcept>
#include <sqlite3.h>

SQLiteAccountRepository::SQLiteAccountRepository(
    const std::string& databaseFile
)
    : storage_(createStorage(databaseFile))
{
    storage_.on_open = [](sqlite3* db) {
        if (sqlite3_exec(
            db,
            "PRAGMA foreign_keys = ON;",
            nullptr,
            nullptr,
            nullptr
        ) != SQLITE_OK) {
            throw std::runtime_error(
                "Cannot enable foreign key constraints"
            );
        }
        };

    storage_.sync_schema();
}

int SQLiteAccountRepository::add(
    const Account& account
) {
    return storage_.insert(account);
}

std::optional<Account>
SQLiteAccountRepository::findById(int id) {
    return storage_.get_optional<Account>(id);
}

std::vector<Account>
SQLiteAccountRepository::findAll() {
    return storage_.get_all<Account>();
}

bool SQLiteAccountRepository::update(
    const Account& account
) {
    auto existing =
        storage_.get_optional<Account>(account.id);

    if (!existing) {
        return false;
    }

    storage_.update(account);
    return true;
}

bool SQLiteAccountRepository::remove(int id) {
    auto existing =
        storage_.get_optional<Account>(id);

    if (!existing) {
        return false;
    }

    storage_.remove<Account>(id);
    return true;
}