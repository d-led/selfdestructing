#include "selfdestructing/selfdestructing.hpp"
#include <iostream>

/* 
    g++ -std=c++11 -I. simple_test.cpp -o simple_test -pthread // ./simple_test
*/

struct Tracer : public crashes::on<2>::copies {};

struct TestCrash {
    Tracer _tracer;    
};

int main() {
    try {
        TestCrash c1;
        TestCrash c2 = c1;  // Should succeed
        TestCrash c3 = c1;  // Should throw
        std::cout << "Second copy succeeded - this shouldn't have happened" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Caught an expected exception: " << e.what() << std::endl;
    }
    
    return 0;
}
