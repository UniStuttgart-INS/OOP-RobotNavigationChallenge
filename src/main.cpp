#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#include "internal/GameApplication.hpp"

int main(int argc, const char* argv[])
{
    auto console_sink = spdlog::stdout_color_mt("console");
    console_sink->set_level(spdlog::level::trace);
    console_sink->set_pattern("[%H:%M:%S.%e] [%^%L%$] %v");
    spdlog::set_default_logger(console_sink);

    oop::internal::GameApplication app("INS - OOP Robot Navigation Challenge", "ImGui.ini", argc, argv);

    if (app.Create())
    {
        return app.Run();
    }

    return EXIT_FAILURE;
}