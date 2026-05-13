#include "core/application.h"

int main(int argc, char* argv[])
{
    Actinium::GApp = new Actinium::Application(argc, argv);

    const auto result = Actinium::GApp->Run();

    delete Actinium::GApp;

    return result;
}
