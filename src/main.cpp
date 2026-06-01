#include "core/application.h"

int main(const int argc, char* argv[])
{
    Actinium::Application app(argc, argv);

    // ReSharper disable once CppDFALocalValueEscapesFunction
    Actinium::GApp = &app;

    const auto result = app.Run();

    Actinium::GApp = nullptr;

    return result;
}
