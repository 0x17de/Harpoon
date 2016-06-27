#include <iostream>
#include <algorithm>
#include <array>
#include "app/Application.hpp"
#include "utils/Password.hpp"
#include "utils/Ini.hpp"
#include "utils/IdProvider.hpp"

using namespace std;


bool doGenUser(bool save) {
    string username, password;

    cout << "Username: ";
    cin >> username;

    cout << "Password: ";
    cin >> password;

    Password p(password);
    if (save) {
        Ini users("config/users.ini");
        auto& user = users.expectCategory(username);

        string userIdString;
        users.getEntry(user, "id", userIdString);
        if (userIdString.size() == 0)
            users.setEntry(user, "id", to_string(IdProvider::getInstance().generateNewId("user")));

        users.setEntry(user, "salt", p.getSaltBase64());
        users.setEntry(user, "password", p.getHashBase64());
        cout << "users.ini updated." << endl;
    } else {
        cout << "[" << username << "]" << endl
             << "salt=" << p.getSaltBase64() << endl
             << "password=" << p.getHashBase64() << endl;
    }
}

bool doSetup(bool save) {
    bool usersIniExists;
    {
        Ini users("config/users.ini");
        usersIniExists = !users.isNew();
    }

    string loginDatabaseType,
		enableWebChat,
		enableIrcService;
    static const array<string, 2> validLoginDatabaseTypes{"dummy", "ini"};
    static const array<string, 2> validYesNoAnswers{"y", "n"};

    do {
        cout << "Login database type (dummy/ini) [ini]: ";
        getline(cin, loginDatabaseType);
        if (loginDatabaseType.size() == 0) // auto value
            loginDatabaseType = "ini";
    } while (find(validLoginDatabaseTypes.begin(), validLoginDatabaseTypes.end(), loginDatabaseType) == validLoginDatabaseTypes.end());

    do {
        cout << "Enable IRC service (y/n) [y]: ";
        getline(cin, enableIrcService);
        if (enableIrcService.size() == 0) // auto value
            enableIrcService = "y";
    } while (find(validYesNoAnswers.begin(), validYesNoAnswers.end(), enableIrcService) == validYesNoAnswers.end());

    do {
        cout << "Enable WebChat (y/n) [y]: ";
        getline(cin, enableWebChat);
        if (enableWebChat.size() == 0) // auto value
            enableWebChat = "y";
    } while (find(validYesNoAnswers.begin(), validYesNoAnswers.end(), enableWebChat) == validYesNoAnswers.end());

    // write core configuration
    {
        Ini core("config/core.ini");

        auto& modules = core.expectCategory("modules");
        core.setEntry(modules, "login", loginDatabaseType);
        core.setEntry(modules, "webchat", enableWebChat);

        auto& services = core.expectCategory("services");
        core.setEntry(services, "irc", enableIrcService);
    }

    // create first user if no users.ini exists
    if (!usersIniExists) {
        cout << endl << "The first can now be created" << endl;
        doGenUser(save);
    }
}

bool checkArgs(int argc, char** argv) {
    bool help = false,
		setup = false,
		genUser = false,
		save = false;

    // check parameters
    for (int currentArgI = 1; currentArgI < argc; ++currentArgI) {
        string arg(argv[currentArgI]);
        if (arg == "--setup") setup = true;
        if (arg == "--genuser") genUser = true;
        if (arg == "-h" || arg == "--help") help = true;
        if (arg == "--save") save = true;
    }

    // actions
    if (help) {
        cout << "Valid commands:" << endl
             << "  --setup     Generate initial configuration. (Useful with --save)" << endl
             << "  --genuser   Generate some user account. (Useful with --save)" << endl
             << "  --save      Save the configured data into the corresponding ini file. (No process must be running)" << endl;
        return false;
    }
    if (setup) {
        doSetup(save);
        return false;
    }
    if (genUser) {
        doGenUser(save);
        return false;
    }
    return true;
}

int main(int argc, char** argv) {
    if (!checkArgs(argc, argv))
        return 0;

    try {
        Application app;
        app.join();
    } catch(runtime_error& run) {
        cout << run.what() << endl;
    }

    std::cout << "Application stopped" << std::endl;
    return 0;
}

