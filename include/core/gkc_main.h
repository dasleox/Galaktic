/*
  Galaktic Engine
  Copyright (C) 2026 SummerChip

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#pragma once

#include <vector>
#include <memory>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <SDL3/SDL.h>
#include <iostream>
#include <filesystem>
#include <typeindex>
#include <any>

// Operating System Detection

#if __WIN64
#define GKC_OS "Windows"
#define GKC_OS_INT 0
#include <windows.h>
#include <psapi.h>
#elif __linux__
#define GKC_OS "Linux"
#define GKC_OS_INT 1
#elif __APPLE__
#error "We dont support apple yet! :("
#endif

// Architecture detection

#if __i386__
#error "32-bit architecture is not supported in the Galaktic Engine!"
#elif __amd64__ || __x86_64__
#define GKC_ARCH "x64"
#endif


#define GKC_RELEASE_ASSERT(_Expression, msg)        \
    do {                                            \
        if (!(_Expression)) {                       \
            GKC_ENGINE_FATAL("Assertion '{0}' | {1}", #_Expression, msg); \
            std::abort();                           \
        }                                           \
    } while (0)

// Compile definitions
#ifdef GKC_ENABLE_ASSERTS
    /**
     * @param _Expression The expression to check
     * @param msg The message to print before aborting
     * @warning This macro ONLY has to be used after calling
     *          Galaktic::Debug::Logger::Init(), using before this function
     *          may cause undefined behaviour (crashes!)
     */
    #define GKC_ASSERT(_Expression, msg)                \
        do {                                            \
            if (!(_Expression)) {                       \
                GKC_ENGINE_FATAL("Assertion '{0}' | {1}", #_Expression, msg); \
                std::abort();                           \
            }                                           \
        } while (0)
#else
    #define GKC_ASSERT(_Expression, msg) ((void)0)

#endif

#define GKC_MAJOR_VERSION 0
#define GKC_MINOR_VERSION 3
#define GKC_PATCH_VERSION 0
#define GKC_SET_COLOR(color) color.r, color.g, color.b, color.a

// 'using' declarations
using std::shared_ptr, std::unique_ptr, std::weak_ptr, std::cout, std::cin, std::endl;
using std::string, std::to_string, std::vector, std::map, std::unordered_map, std::unordered_set;
using std::stringstream, std::ifstream, std::ofstream;
using std::make_shared, std::make_unique, std::function;
using std::filesystem::path, std::unordered_multimap;
using std::type_index, std::any, std::array;

const string GKC_SUFFIX = "Earthy";
const Uint32 GKC_BUILD_VERSION = 396;
inline const string GKC_VERSION_STR = to_string(GKC_MAJOR_VERSION) + "."
    + to_string(GKC_MINOR_VERSION) + "." + to_string(GKC_PATCH_VERSION);

typedef Uint32 EntityID;
typedef Uint32 AudioID;
typedef Uint32 GKC_WindowID;
typedef Uint32 ComponentTypeID;
typedef Uint32 TextureID;
typedef Uint32 ScriptID;
typedef Uint32 AnimationID;

inline constexpr Uint32 MAX_WINDOW_QUANTITY = 64;
inline constexpr EntityID InvalidEntity = 0;
inline constexpr double FIXED_DELTA_TIME = 1.0 / 60;

#if GKC_OS_INT == 0
    inline const path GKC_TEXTURE_PATH = "assets\\textures";
    inline const path GKC_SOUND_PATH = "assets\\sounds";
    inline const path GKC_ANIMATION_PATH = "assets\\animations";
    inline const path GKC_SCENE_PATH = "scenes";
    inline const path GKC_SCRIPT_PATH = "scripts";
    inline const path GKC_CONFIG_PATH = "config";
#else
    inline const path GKC_TEXTURE_PATH = "assets/textures";
    inline const path GKC_SOUND_PATH = "assets/sounds";
    inline const path GKC_ANIMATION_PATH = "assets/animations"
    inline const path GKC_SCENE_PATH = "assets/scenes"
    inline const path GKC_SCRIPT_PATH = "scripts";
    inline const path GKC_CONFIG_PATH = "config";
#endif

inline constexpr SDL_Color WHITE_COLOR = {255, 255, 255, 255};
inline constexpr SDL_Color BLACK_COLOR = {0, 0, 0, 255};
inline constexpr SDL_Color RED_COLOR = {255, 0, 0, 255};
inline constexpr SDL_Color GREEN_COLOR = {0, 255, 0, 255};
inline constexpr SDL_Color BLUE_COLOR = {0, 0, 255, 255};
inline constexpr SDL_Color YELLOW_COLOR = {255, 255, 0, 255};
inline constexpr SDL_Color MAGENTA_COLOR = {255, 0, 255, 255};
inline constexpr SDL_Color CYAN_COLOR = {0, 255, 255, 255};
inline constexpr SDL_Color GREY_COLOR = {128, 128, 128, 255}; 

namespace Galaktic::Render {
    /**
     * @struct Vec2
     * @brief Vector with X,Y values (2D)
     */
    struct Vec2 {
        float x;
        float y;

        Vec2 operator + (const Vec2& o) const {
            return {x + o.x, y + o.y};
        }
        Vec2 operator - (const Vec2& o) const {
            return {x - o.x, y - o.y};
        }
        Vec2 operator * (const float o) const {
            return { x * o, y * o};
        }
        Vec2 operator / (const float o) const {
            return {x / o, y/ o};
        }

        Vec2& operator += (const Vec2 o) {
            x += o.x, y += o.y; return *this;
        };
    };
}

namespace Galaktic::Core {
    /**
     * @brief Struct to hold device information such as screen width, height, OS, and architecture.
     * @struct DeviceInformation
    */
    struct DeviceInformation {
          string os_;         // Operating system of the computer
          string arch_;       // Architecture of the computer
          Uint32 width_;      // Width of the screen
          Uint32 height_;     // Height of the screen
          [[nodiscard]] bool IsCorrupted() const {
              return os_.empty() || arch_.empty() || width_ >= 16000 || height_ >= 16000;
          }
    };

    /**
     * @brief Checks if the weak_ptr passed has expired
     * @tparam T typename of weak_ptr
     * @param ptr Weak pointer of type T
     * @return true if the pointer expired, false otherwise
     */
    template <typename T>
    bool IsWeakPtrExpired(const weak_ptr<T>& ptr) {
        if (ptr.expired()) {
            return true;
        }
        return false;
    }

    /**
     * @brief Converts the SDL_Color struct to a HEX string
     * @param color SDL_Color
     * @return HEX string
     */
    inline std::string SDL_ColorToHEX(SDL_Color color) {
        std::stringstream ss;
        ss << "#"
           << std::hex << std::uppercase << std::setfill('0')
           << std::setw(2) << static_cast<int>(color.r)
           << std::setw(2) << static_cast<int>(color.g)
           << std::setw(2) << static_cast<int>(color.b)
           << std::setw(2) << static_cast<int>(color.a);
        return ss.str();
    }

    /**
     * @brief Converts a HEX string to a SDL_Color struct
     * @param hex HEX String
     * @return A SDL_Color struct
     */
    inline SDL_Color HexToSDL_Color(const string& hex) {
        string s = hex;

        if (s.starts_with("#"))
            s.erase(0, 1);
        else if (s.starts_with("0x") || s.starts_with("0X"))
            s.erase(0, 2);

        if (s.length() != 6 && s.length() != 8)
            return WHITE_COLOR;

        SDL_Color color;
        color.r = static_cast<Uint8>(std::stoul(s.substr(0, 2), nullptr, 16));
        color.g = static_cast<Uint8>(std::stoul(s.substr(2, 2), nullptr, 16));
        color.b = static_cast<Uint8>(std::stoul(s.substr(4, 2), nullptr, 16));
        color.a = s.length() == 8 ? static_cast<Uint8>(std::stoul(s.substr(6, 2), nullptr, 16)) : 255;
        return color;
    }

    /**
     * @brief Cast a variable of type T to a void pointer
     * @tparam T typename
     * @param value value
     * @return A void pointer
     * @note This function is mostly used in Galaktic as a way of printing addresses of non-primitive
     *       variables in spdlog
     */
    template <typename T>
    const void* CastToVoidPtr(const T& value) {
        return static_cast<const void*>(&value);
    }

    /**
     * @brief Returns a string with the X and Y value of the passed Vec2
     * @param vec Vec2
     * @return A string with X and Y value
     */
    inline string Vec2ToString(const Render::Vec2& vec) {
        return "X: " + std::to_string(vec.x) + "," + " Y: " + std::to_string(vec.y);
    }
    
    inline std::string StripExtension(const std::string& filename) {
        size_t pos = filename.find_last_of(".");
        if (pos == std::string::npos)
            return filename;
        return filename.substr(0, pos);
    }

    template<typename Key, typename Value>
    inline string GenerateUniqueName(const unordered_map<Key, Value>& map, const string& baseName) {
        string uniqueName = baseName;
        int counter = 2;
        while (map.find(uniqueName) != map.end()) {
            uniqueName = baseName + std::to_string(counter);
            counter++;
        }
        return uniqueName;
    }
}

/**
 * @brief A macro to insert X and Y
 * @param vec Vec2
 */
#define GKC_VEC2_DECOUPLE(vec) vec.x, vec.y

namespace Galaktic::Debug {
    #if GKC_OS_INT == 0
    inline size_t GetGalakticRAMUsage() {
            PROCESS_MEMORY_COUNTERS pmc{};
            if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc)))
            {
                return pmc.WorkingSetSize / (1024 * 1024);  // MB
            }
            return 0;
        }

    inline size_t GetSystemRAM() {
            MEMORYSTATUSEX mem{};
            mem.dwLength = sizeof(mem);
            GlobalMemoryStatusEx(&mem);
            return mem.ullTotalPhys / (1024 * 1024); // MB
        }
    #elif GKC_OS_INT == 1
        /*
        size_t GetGalakticRAMUsage() {
            std::ifstream file("/proc/self/status");
            std::string line;

            while (std::getline(file, line)) {
                if (line.rfind("VmRSS:", 0) == 0) {
                    return std::stoul(line.substr(6)) / 1024; // MB
                }
            }
            return 0;
        }

        size_t GetSystemRAM() {
            std::ifstream file("/proc/meminfo");
            std::string key;
            size_t value;
            std::string unit;

            while (file >> key >> value >> unit) {
                if (key == "MemTotal:") {
                    return value / 1024; // MB
                }
            }
            return 0;
        }
        */
    #endif
}

namespace Galaktic::Filesystem {
    constexpr unsigned int GKC_VERSION_ENTITY = 1;
    constexpr unsigned int GKC_VERSION_SCENE = 1;
}



#define GKC_ENSURE_FILE_OPEN(file, ex)                          \
    do {                                                        \
        if (!(file).is_open()) {                                \
            GKC_THROW_EXCEPTION(ex, "file is not open!");       \
            }                                                   \
    } while (0)        

#define GKC_WRITE_BINARY(var) reinterpret_cast<const char*>(&var)
#define GKC_READ_BINARY(var) reinterpret_cast<char*>(&var)