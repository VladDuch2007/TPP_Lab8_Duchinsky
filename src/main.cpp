
#include <iostream>
#include <exception>

#include "TestData.h"
#include "Queries.h"
#include "Transactions.h"
#include "Comparison.h"
#include "RepositoryDemo.h"

int main() {
    try {
        runRepositoryDemo();

        prepareTestData();

        runQueryDemo();

        runTransactionDemo();

        runWithoutTransactionDemo();

        runComparisonDemo();

        return 0;
    }
    catch (const std::exception& ex) {
        std::cerr << "Application error: "
            << ex.what() << '\n';

        return 1;
    }
}