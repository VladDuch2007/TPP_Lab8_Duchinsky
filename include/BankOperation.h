
#pragma once

#include <string>

struct BankOperation {
    int id = 0;
    int accountId = 0;
    std::string operationDate;
    std::string operationType;
    double amount = 0.0;
    std::string description;
};