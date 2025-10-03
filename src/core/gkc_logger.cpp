#include <core/gkc_logger.h>
#include <pch.hpp>

using namespace Galaktic::Debug;

namespace Galaktic {
    namespace Debug {
        std::shared_ptr<spdlog::logger> Logger::engine_logger;
        std::shared_ptr<spdlog::logger> Logger::client_logger;
    }
}


void Logger::Init() {
    // [Warning] [09/28/25--23:25:40] [your_mom.cpp -> inMyBed || Line: 69] [Leonardo]: Hi :333!
    spdlog::set_pattern("[%l] [%D--%X] [%s -> %! || Line: %#] [%n]: %v ");

    engine_logger = spdlog::stdout_color_mt("GALAKTIC");
    engine_logger->set_level(spdlog::level::trace);

    client_logger = spdlog::stdout_color_mt("CLIENT");
    client_logger->set_level(spdlog::level::trace);
}