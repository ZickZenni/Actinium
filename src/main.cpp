#include "build_config.h"
#include "core/application.h"
#include "core/logger.h"
#include "util/std/string.h"

int main(const int argc, char* argv[])
{
    Actinium::Logger::Init();
    Actinium::Logger::Debug("root", "Starting up on version {}", VERSION);
    Actinium::Logger::Debug(
        "root", "Arguments: [\"{}\"]", Actinium::String::Join(Actinium::String::ToVector(argc, argv), "\", \""));

    Actinium::Application app(argc, argv);

    // ReSharper disable once CppDFALocalValueEscapesFunction
    Actinium::GApp = &app;

    const auto result = app.Run();

    Actinium::GApp = nullptr;
    Actinium::Logger::Info("root", "Shutting down (code={})", result);

    return result;
}
