#include <iostream>
#include "app/Application.hpp"
#include "utils/Password.hpp"
#include "utils/Ini.hpp"
#include "utils/IdProvider.hpp"

using namespace std;


bool checkArgs(int argc, char** argv) {
	bool help = false,
	     genUser = false,
	     save = false;

	// check parameters
	for (int currentArgI = 1; currentArgI < argc; ++currentArgI) {
		string arg(argv[currentArgI]);
		if (arg == "--genuser") genUser = true;
		if (arg == "-h" || arg == "--help") help = true;
		if (arg == "--save") save = true;
	}

	// actions
	if (help) {
		cout << "Valid commands:" << endl
			<< "  --genuser   Generate some user account. (Useful with --save)" << endl
			<< "  --save      Save the user account into the users.ini. (No process must be running)" << endl;
		return false;
	}
	if (genUser) {
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

		return false;
	}
	return true;
}

int main(int argc, char** argv) {
	if (!checkArgs(argc, argv))
		return 0;

	Application app;
	app.join();
	std::cout << "Application stopped" << std::endl;
	return 0;
}

