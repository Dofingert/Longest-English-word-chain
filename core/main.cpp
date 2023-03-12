#include <iostream>
#include "gtest/gtest.h"

int fut(int n) {
    if(n <= 1) {
        return 1;
    }
    return fut(n - 1) + fut(n - 2);
}

int main(int argc, char *argv[]) {
    std::cout << "Hello, World!" << std::endl;
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
