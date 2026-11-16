#pragma once

#include <string>

#include "IAccountRepository.h"
#include "Database.h"

class SQLiteAccountRepository : public IAccountRepository {
public:
    explicit SQLiteAccountRepository(
        const std::string& databaseFile
    );

    int add(const Account& account) override;

    std::optional<Account> findById(int id) override;

    std::vector<Account> findAll() override;

    bool update(const Account& account) override;

    bool remove(int id) override;

private:
    using Storage = decltype(createStorage(""));

    Storage storage_;
};