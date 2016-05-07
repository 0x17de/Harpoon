#include <iostream>
#include "app/Application.hpp"

int main() {
	Application app;
	app.join();
	std::cout << "Application stopped" << std::endl;
	return 0;
}

