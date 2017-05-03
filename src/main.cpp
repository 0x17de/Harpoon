#include <iostream>
#include <algorithm>
#include <array>
#include <sstream>
#include "app/Application.hpp"
#include "app/ArgumentParser.hpp"

using namespace std;


int main(int argc, char** argv) {
    // Includes check for setup flags for generation of the config files
    if (!ArgumentParser::checkArgs(argc, argv))
        return 0;

    try {
        Application app;
    } catch(runtime_error& run) {
        cout << run.what() << endl;
    }

    std::cout << "Application stopped" << std::endl;
    return 0;
}

