#include <iostream>
#include <vector>

#include "linq.h"

using namespace pk;

int main()
{
    std::vector<int> v = {1, 2, 3, 4, 5};
    auto where_result = v >> where([&](const int& value){ return value > 2; });
    auto select_result = v >> select([&](const int& value){ return value % 2 == 0; });

    return 0;
}