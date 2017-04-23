#ifndef APPLICATIONGUARD_H
#define APPLICATIONGUARD_H

#include <list>


class Application;
/// Sets the signal handler and sets the application instance variable used by the handler
class ApplicationGuard {
public:
    /// List of applications using the signal handler
    static std::list<Application*> apps;
    Application* app;
    /// Sets the handler and inserts the application into the list
    explicit ApplicationGuard(Application* app);
    /// Removes the application from the list but keeps the signal handlers
    ~ApplicationGuard();
};

#endif
