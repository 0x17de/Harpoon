#ifndef APPLICATIONGUARD_H
#define APPLICATIONGUARD_H

#include <list>


class Application;
class ApplicationGuard {
public:
	static std::list<Application*> apps;
	Application* app;
	ApplicationGuard(Application* app);
	~ApplicationGuard();
};

#endif
