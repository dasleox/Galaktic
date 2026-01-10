#include <core/gkc_logger.h>
using namespace Galaktic::Debug;

namespace Galaktic::Debug {
    std::shared_ptr<spdlog::logger> Logger::engine_logger_;
    std::shared_ptr<spdlog::logger> Logger::client_logger_;
}

void Logger::Init() {
    // [Warning] [09/28/25--23:25:40] [your_mom.cpp -> inMyBed || Line: 69] [Leonardo]: Hi :333!
    spdlog::set_pattern("[%l] [%D--%X] [%s -> %! || Line: %#] [%n]: %v ");

    engine_logger_ = spdlog::stdout_color_mt("GALAKTIC");
    engine_logger_->set_level(spdlog::level::trace);

    client_logger_ = spdlog::stdout_color_mt("CLIENT");
    client_logger_->set_level(spdlog::level::trace);
}

void Logger::PrintEngineInformation() {
    cout << "Galaktic Engine " << GKC_VERSION << " '"
        << GKC_SUFFIX << "' " << "Build " << GKC_BUILD_VERSION << endl;
}

string Logger::GetEngineName() {
    return "Galaktic Engine " + GKC_VERSION + " '"
        + GKC_SUFFIX + "' " + "Build " + to_string(GKC_BUILD_VERSION);
}
string Logger::GetDisplayInfo(const Core::DeviceInformation& deviceInfo) {
    return to_string(deviceInfo.width_) + "x" + to_string(deviceInfo.height_);
}