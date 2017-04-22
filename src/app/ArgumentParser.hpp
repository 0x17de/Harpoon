#ifndef ARGUMENTPARSER_H
#define ARGUMENTPARSER_H

#include <iostream>
#include <algorithm>
#include <string>
#include <array>
#include <sstream>
#include "utils/Password.hpp"
#include "utils/Ini.hpp"
#include "utils/IdProvider.hpp"
#include "utils/Filesystem.hpp"
#include "version.hpp"

class ArgumentParser {
public:
    static void doGenUser(bool save);
    static void doSetup(bool save);
    static bool checkArgs(int argc, char** argv);

    template<class T, class R>
    inline static bool isValidChoice(const T& validChoices, const R& choice) {
        auto end = validChoices.cend();
        return std::find(validChoices.cbegin(), end, choice) != end;
    }

    template<class T>
    inline static void getChoice(const std::string& message, const T& validChoices, std::string& choice, const std::string& defaultValue) {
        do {
            std::cout << message;
            std::getline(std::cin, choice);
            if (choice.size() == 0) // auto value
                choice = defaultValue;
        } while(!isValidChoice(validChoices, choice));
    }
};

#endif
