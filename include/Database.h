
#pragma once

#include <string>
#include <sqlite_orm/sqlite_orm.h>

#include "Account.h"
#include "BankOperation.h"

inline auto createStorage(const std::string& filename) {
    using namespace sqlite_orm;

    return make_storage(
        filename,

        make_table(
            "Accounts",

            make_column(
                "id",
                &Account::id,
                primary_key().autoincrement()
            ),

            make_column(
                "owner",
                &Account::owner,
                not_null()
            ),

            make_column(
                "accountNumber",
                &Account::accountNumber,
                not_null()
            ),

            make_column(
                "currency",
                &Account::currency,
                not_null()
            ),

            make_column(
                "balance",
                &Account::balance
            )
        ),

        make_table(
            "BankOperations",

            make_column(
                "id",
                &BankOperation::id,
                primary_key().autoincrement()
            ),

            make_column(
                "accountId",
                &BankOperation::accountId
            ),

            make_column(
                "operationDate",
                &BankOperation::operationDate,
                not_null()
            ),

            make_column(
                "operationType",
                &BankOperation::operationType,
                not_null()
            ),

            make_column(
                "amount",
                &BankOperation::amount
            ),

            make_column(
                "description",
                &BankOperation::description,
                not_null()
            ),

            foreign_key(
                &BankOperation::accountId
            ).references(
                &Account::id
            )
        )
    );
}