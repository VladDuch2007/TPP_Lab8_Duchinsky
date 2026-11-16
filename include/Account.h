
#pragma once

#include <string>

struct Account {
    int id = 0;
    std::string owner;
    std::string accountNumber;
    std::string currency;
    double balance = 0.0;
};