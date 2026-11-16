#pragma once

#include <optional>
#include <vector>

#include "Account.h"

class IAccountRepository {
public:
    virtual ~IAccountRepository() = default;

    virtual int add(const Account& account) = 0;

    virtual std::optional<Account> findById(int id) = 0;

    virtual std::vector<Account> findAll() = 0;

    virtual bool update(const Account& account) = 0;

    virtual bool remove(int id) = 0;
};