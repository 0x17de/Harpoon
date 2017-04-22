#include "ArgumentParser.hpp"

using namespace std;


void ArgumentParser::doGenUser(bool save) {
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
        if (userIdString.size() == 0) {
            userIdString = to_string(IdProvider::getInstance().generateNewId("user"));
            users.setEntry(user, "id", userIdString);
        }

        stringstream userDirectory;
        userDirectory << "config/user" << userIdString;
        Filesystem::getInstance().createPathRecursive(userDirectory.str());

        users.setEntry(user, "salt", p.getSaltBase64());
        users.setEntry(user, "password", p.getHashBase64());
        cout << "users.ini updated." << endl;
    } else {
        cout << "[" << username << "]" << endl
             << "salt=" << p.getSaltBase64() << endl
             << "password=" << p.getHashBase64() << endl;
    }
}

void ArgumentParser::doSetup(bool save) {
    Filesystem::getInstance().createPathRecursive("config");

    bool usersIniExists;
    {
        Ini users("config/users.ini");
        usersIniExists = !users.isNew();
    }

    string loginDatabaseType,
        backlogDatabaseType,
        ircSettingsDatabaseType,
        hackSettingsDatabaseType,
        enableWebChat,
        enableIrcService,
        enableIrcBacklog,
        enableHackService,
        enableHackBacklog;
    static const array<string, 2> validBacklogDatabaseTypes{{"none", "postgres"}};
    static const array<string, 2> validLoginDatabaseTypes{{"dummy", "ini"}};
    static const array<string, 2> validIrcDatabaseTypes{{"dummy", "ini"}};
    static const array<string, 2> validYesNoAnswers{{"y", "n"}};

    getChoice("Login database type (dummy/ini) [ini]: ", validLoginDatabaseTypes, loginDatabaseType, "ini");
    getChoice("Backlog database type (none/postgres) [postgres]: ", validBacklogDatabaseTypes, backlogDatabaseType, "postgres");

    getChoice("Enable IRC service (y/n) [y]: ", validYesNoAnswers, enableIrcService, "y");
    if (enableIrcService == "y") {
        getChoice("IRC settings database type (dummy/ini) [ini]: ", validIrcDatabaseTypes, ircSettingsDatabaseType, "ini");
        getChoice("Collect IRC backlog (y/n) [y]: ", validYesNoAnswers, enableIrcBacklog, "y");
    }

    getChoice("Enable WebChat (y/n) [y]: ", validYesNoAnswers, enableWebChat, "y");

    // write core configuration
    {
        Ini core("config/core.ini");

        auto& modules = core.expectCategory("modules");
        core.setEntry(modules, "login", loginDatabaseType);
        core.setEntry(modules, "webchat", enableWebChat);
        core.setEntry(modules, "database", backlogDatabaseType);

        auto& services = core.expectCategory("services");
        core.setEntry(services, "irc", enableIrcService);
        core.setEntry(services, "hack", enableHackService);
    }

    // write irc configuration
    if (enableIrcService == "y") {
        Ini irc("config/irc.ini");

        auto& modules = irc.expectCategory("modules");
        irc.setEntry(modules, "settings_database", ircSettingsDatabaseType);
        irc.setEntry(modules, "backlog", enableIrcBacklog);
    }

    // write hack configuration
    if (enableHackService == "y") {
        Ini hack("config/hack.ini");

        auto& modules = hack.expectCategory("modules");
        hack.setEntry(modules, "settings_database", hackSettingsDatabaseType);
        hack.setEntry(modules, "backlog", enableHackBacklog);
    }

    // create first user if no users.ini exists
    if (!usersIniExists) {
        cout << endl << "The first can now be created" << endl;
        doGenUser(save);
    }
}

bool ArgumentParser::checkArgs(int argc, char** argv) {
    bool help = false,
        setup = false,
        genUser = false,
        save = false,
        version = false;

    // check parameters
    for (int currentArgI = 1; currentArgI < argc; ++currentArgI) {
        string arg(argv[currentArgI]);
        if (arg == "-v" || arg == "--version") version = true;
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
    if (version) {
        cout << "Harpoon v" << VERSION_MAJOR << "." << VERSION_MINOR << "." << VERSION_PATCH << endl;
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
