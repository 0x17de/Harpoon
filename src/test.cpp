#include "test.hpp"
#include <iostream>
#include <sstream>


int main() {
    std::cout << "Starting tests" << std::endl;
    TestRunner::getInstance().run();
    std::cout << "Tests run successful" << std::endl;

    return 0;
}


TestError::TestError(const std::string& file, size_t line)
    : runtime_error(std::string("Error in file \"")+file+"\" at line "+std::to_string(line))
{
}

TestRunner::TestRunner() {
}

TestRunner& TestRunner::getInstance() {
    static TestRunner testRunner;
    return testRunner;
}

void TestRunner::run() {
    for (auto& testData : tests) {
        try {
            std::cout << std::get<0>(testData) << ": " << std::get<1>(testData) << std::endl;
            std::get<2>(testData)();
        } catch(std::exception& e) {
            std::cout << e.what() << std::endl;
        }
    }
}
