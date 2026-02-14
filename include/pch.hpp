#pragma once

#include <vector>
#include <memory>
#include <stdexcept>
#include <exception>
#include <chrono>
#include <functional>
#include <ctime>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <iomanip>
#include <string>
#include <SDL3/SDL.h>
#include <../libs/SDL_mixer/include/SDL3_mixer/SDL_mixer.h>
#include <SDL3/SDL_video.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <core/gkc_main.h>
extern "C" {
    #include <lua.h>
    #include <lauxlib.h>
    #include <lualib.h>
}
#include <LuaBridge/LuaBridge.h>
#include <typeindex>
#include <cxxabi.h>