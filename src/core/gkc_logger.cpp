#include <core/gkc_logger.h>
using namespace Galaktic::Debug;

namespace Galaktic::Debug {
    std::shared_ptr<spdlog::logger> Logger::engine_logger_;
    std::shared_ptr<spdlog::logger> Logger::client_logger_;
}

void Logger::Init() {
    #if GKC_DEBUG
        // [Warning] [09/28/25--23:25:40] [your_mom.cpp -> inMyBed || Line: 69] [Leonardo]: Hi :333!
        spdlog::set_pattern("%^[%l]%$ [%D--%X] [%s -> %! || Line: %#] [%n]: %v ");
    #else
        // [Warning] [09/28/25--23:25:40] [Leonardo]: Hi :333!
        spdlog::set_pattern("%^[%l]%$ [%D--%X] [%n]: %v ");
    #endif

    engine_logger_ = spdlog::stdout_color_mt("GALAKTIC");
    client_logger_ = spdlog::stdout_color_mt("CLIENT");
}

void Logger::PrintEngineInformation() {
    cout << "Galaktic Engine " << GKC_VERSION_STR << " '"
        << GKC_SUFFIX << "' " << "Build " << GKC_BUILD_VERSION << endl;
}

string Logger::GetEngineName() {
    return "Galaktic Engine " + GKC_VERSION_STR + " '"
        + GKC_SUFFIX + "' " + "Build " + to_string(GKC_BUILD_VERSION);
}
string Logger::GetDisplayInfo(const Core::DeviceInformation& deviceInfo) {
    return to_string(deviceInfo.width_) + "x" + to_string(deviceInfo.height_);
}

string Logger::DemangleTypename(const char *name) {
    int status = -1;
    unique_ptr<char, void(*)(void*)> res = {
        abi::__cxa_demangle(name, NULL, NULL, &status),
        std::free
    };
    return (status == 0) ? res.get() : name;
}
