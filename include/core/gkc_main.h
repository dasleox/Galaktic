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
#include <algorithm>
#include <cctype>
#include <memory>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <SDL3/SDL.h>
#include <iostream>
#include <filesystem>
#include <typeindex>
#include <cxxabi.h>
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
#define GKC_MINOR_VERSION 7
#define GKC_PATCH_VERSION 0
#define GKC_SET_COLOR(color) (color.r), (color.g), (color.b), (color.a)

// 'using' declarations
using std::shared_ptr, std::unique_ptr, std::weak_ptr, std::cout, std::cin, std::endl;
using std::string, std::to_string, std::vector, std::map, std::unordered_map, std::unordered_set;
using std::stringstream, std::ifstream, std::ofstream;
using std::make_shared, std::make_unique, std::function;
using std::filesystem::path, std::unordered_multimap;
using std::type_index, std::any, std::array, std::move;

const string GKC_SUFFIX = "Earthy";
inline const string GKC_VERSION_STR = to_string(GKC_MAJOR_VERSION) + "."
    + to_string(GKC_MINOR_VERSION) + "." + to_string(GKC_PATCH_VERSION);

typedef Uint32 EntityID;
typedef Uint32 AudioID;
typedef Uint32 GKC_WindowID;    
typedef Uint32 ComponentTypeID;
typedef Uint32 TextureID;
typedef Uint32 ScriptID;
typedef Uint32 AnimationID;
typedef Uint32 PixelGroupID;
typedef Uint32 FontID;

inline constexpr float GKC_PI = 3.14159265358979323846f;

inline constexpr Uint32 MAX_WINDOW_QUANTITY = 64;
inline constexpr Uint32 MAX_ENTITY_QUANTITY = 4096 * 8;
inline constexpr Uint32 MAX_COMPONENT_QUANTITY = MAX_ENTITY_QUANTITY * 8;
inline constexpr Uint32 MAX_TEXTURE_QUANTITY = 8192;
inline constexpr Uint32 MAX_ANIMATION_QUANTITY = 8192;
inline constexpr Uint32 MAX_SOUNDFILE_QUANTITY = 4096;
inline constexpr Uint32 MAX_SCRIPT_QUANTITY = 1024;
inline constexpr Uint32 MAX_TRACKS_QUANTITY = 8192 * 2;
inline constexpr Uint32 MAX_FONT_QUANTITY = 1024;

inline constexpr EntityID InvalidEntity = 0;
inline constexpr double FIXED_DELTA_TIME = 1.0 / 60;

inline const path GKC_TEXTURE_PATH = "assets\\textures";
inline const path GKC_SOUND_PATH = "assets\\sounds";
inline const path GKC_ANIMATION_PATH = "assets\\animations";
inline const path GKC_SCENE_PATH = "scenes";
inline const path GKC_SCRIPT_PATH = "scripts";
inline const path GKC_CONFIG_PATH = "config";

inline const path GKC_ARIAL_FONT = "C:\\Windows\\Fonts\\Arial.ttf";


inline constexpr SDL_Color WHITE_COLOR = {255, 255, 255, 255};
inline constexpr SDL_Color BLACK_COLOR = {0, 0, 0, 255};
inline constexpr SDL_Color RED_COLOR = {255, 0, 0, 255};
inline constexpr SDL_Color GREEN_COLOR = {0, 255, 0, 255};
inline constexpr SDL_Color BLUE_COLOR = {0, 0, 255, 255};
inline constexpr SDL_Color YELLOW_COLOR = {255, 255, 0, 255};
inline constexpr SDL_Color MAGENTA_COLOR = {255, 0, 255, 255};
inline constexpr SDL_Color CYAN_COLOR = {0, 255, 255, 255};
inline constexpr SDL_Color GREY_COLOR = {128, 128, 128, 255}; 

namespace Galaktic
{
    /**
     * @struct AssetInfo
     * @brief Stores an asset's filepath, ownership, load state and last successful file timestamp.
     */
    template<typename AssetType>
    struct AssetInfo 
    {
        /**
         * @brief Stores the asset filepath without loading the asset yet.
         * @param filepath Filepath to load or store
         */
        AssetInfo(const string& filepath) 
            : filepath(filepath) {}

        string filepath;
        unique_ptr<AssetType> asset = nullptr;
        bool isLoaded = false;
        std::filesystem::file_time_type lastWriteTime{};

        /**
         * @brief Loads a replacement asset, retaining the old asset if validation fails.
         * @param filepath Filepath to load or store
         * @param args Arguments forwarded to the requested operation
         * @return true on success, false otherwise
         */
        template<typename... Args>
        bool LoadAsset(const string& filepath, Args&& ...args)
        {
            auto replacement = make_unique<AssetType>(filepath, std::forward<Args>(args)...);
            if constexpr (requires { replacement->IsValid(); }) {
                if (!replacement->IsValid()) return false;
            }
            asset = std::move(replacement);
            std::error_code error;
            lastWriteTime = std::filesystem::last_write_time(filepath, error);
            isLoaded = true;
            return true;
        }

        /**
         * @brief Checks whether the asset has been successfully loaded.
         * @return true if the asset has been successfully loaded, false otherwise
         */
        bool IsLoaded() const
        {
            return isLoaded;
        } 

        /**
         * @brief Returns the stored asset filepath.
         */
        const string& GetFilepath() const { return filepath; }

        /**
         * @brief Returns access to the asset's owning unique_ptr.
         */
        unique_ptr<AssetType>& GetAsset()
        {
            return asset;
        }

        /**
         * @brief Returns access to the asset's owning unique_ptr.
         */
        const unique_ptr<AssetType>& GetAsset() const
        {
            return asset;
        }

        /**
         * @brief Returns a borrowed asset pointer; ownership remains in AssetInfo.
         */
        AssetType* GetRawAsset() 
        {
            return asset.get();
        }

        /**
         * @brief Returns a borrowed asset pointer; ownership remains in AssetInfo.
         */
        const AssetType* GetRawAsset() const 
        {
            return asset.get();
        }
    };
}
namespace Galaktic::Render {
    /**
     * @struct Vec2
     * @brief Vector with X,Y values (2D)
     */
    struct Vec2 {
        float x = 0.f;
        float y = 0.f;

        /**
         * @brief Returns the component-wise sum of two vectors.
         * @param o Vector or scalar operand
         */
        Vec2 operator + (const Vec2& o) const {
            return {x + o.x, y + o.y};
        }
        /**
         * @brief Returns the component-wise difference of two vectors.
         * @param o Vector or scalar operand
         */
        Vec2 operator - (const Vec2& o) const {
            return {x - o.x, y - o.y};
        }
        /**
         * @brief Returns the vector scaled by a scalar.
         * @param o Vector or scalar operand
         */
        Vec2 operator * (const float o) const {
            return { x * o, y * o};
        }
        /**
         * @brief Returns the vector divided by a nonzero scalar.
         * @param o Vector or scalar operand
         */
        Vec2 operator / (const float o) const {
            return {x / o, y/ o};
        }

        /**
         * @brief Adds the vector operand to this vector and returns this instance.
         * @param o Vector or scalar operand
         */
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
          Uint32 width_ = 0;      // Width of the screen
          Uint32 height_ = 0;     // Height of the screen
          /**
           * @brief Checks for missing device identifiers or unsupported display dimensions.
           * @return true on success, false otherwise
           */
          [[nodiscard]] bool IsCorrupted() const {
              return os_.empty() || arch_.empty() || width_ == 0 || height_ == 0 || width_ >= 16000 || height_ >= 16000;
          }
    };
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

        if (!s.empty() && s[0] == '#')
            s.erase(0, 1);
        else if (s.size() >= 2 && s[0] == '0' && (s[1] == 'x' || s[1] == 'X'))
            s.erase(0, 2);

        if (s.length() != 6 && s.length() != 8)
            return WHITE_COLOR;
        if (!std::all_of(s.begin(), s.end(), [](unsigned char ch) { return std::isxdigit(ch) != 0; }))
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
    
    /**
     * @brief Removes the filename's final extension.
     * @param filename Filename, including its extension when present
     */
    inline std::string StripExtension(const std::string& filename) {
        size_t pos = filename.find_last_of(".");
        if (pos == std::string::npos)
            return filename;
        return filename.substr(0, pos);
    }

    /**
     * @brief Appends a numeric suffix until the name is absent from the supplied map.
     * @param map Lookup map containing names already in use
     * @param baseName Preferred name before adding a unique suffix
     */
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

    /**
     * @brief Converts bytes to whole mebibytes using integer division.
     * @param bytes Memory size in bytes
     */
    inline constexpr uint64_t BytesToMegabytes(uint64_t bytes) 
    {
        return (bytes / 1024) / 1024;
    }
}

/**
 * @brief A macro to insert X and Y
 * @param vec Vec2
 */
#define GKC_VEC2_DECOUPLE(vec) vec.x, vec.y

namespace Galaktic::Debug {
    #if GKC_OS_INT == 0
        /**
         * @brief Returns this process's working-set memory usage in whole mebibytes on Windows.
         */
        inline size_t GetGalakticRAMUsage() {
            PROCESS_MEMORY_COUNTERS pmc{};
            if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc)))
            {
                return pmc.WorkingSetSize / (1024 * 1024);  // MB
            }
            return 0;
        }

        /**
         * @brief Returns the system's total physical memory in whole mebibytes on Windows.
         */
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

    /**
    * @brief Gets the engine version
    * @return A string with the engine version
    */
    inline string GetEngineName() {
        return "Galaktic Engine " + GKC_VERSION_STR + " '"
        + GKC_SUFFIX;
    }  

    /**
     * @brief Returns a string with the display information
     * FORMAT: Width x Height @ (Hertz) Hz
     * @param deviceInfo DeviceInformation instance
     * @return A string with the display information
    */
    inline string GetDisplayInfo(const Core::DeviceInformation& deviceInfo) {
        return to_string(deviceInfo.width_) + "x" + to_string(deviceInfo.height_);
    }

    /**
    * @brief Demangle a typename
    * @param name Typename
    * @return name of the typename
    */
    inline string DemangleTypename(const char* name) 
    {
        int status = -1;
        unique_ptr<char, void(*)(void*)> res = {
            abi::__cxa_demangle(name, NULL, NULL, &status),
            std::free
        };
        return (status == 0) ? res.get() : name;
    }

    /**
     * @brief Returns a readable type name with implementation-specific prefixes removed.
     * @param name Name used to identify the requested entry
     */
    inline string GetCleanTypename(const char* name)
    {
        string demangled = DemangleTypename(name);
        size_t pos = demangled.rfind("::");
        if(pos != string::npos)
            demangled = demangled.substr(pos + 2);
        
        return demangled;
    }

    /**
     * @brief Returns a readable type name with implementation-specific prefixes removed.
     */
    template<typename T>
    inline string GetCleanTypename()
    {
        return GetCleanTypename(typeid(T).name());
    }
}

namespace Galaktic::Filesystem {
    constexpr unsigned int GKC_VERSION_SCENE = 3;
}

#define GKC_WRITE_BINARY(x) reinterpret_cast<const char*>(&x)
#define GKC_READ_BINARY(x) reinterpret_cast<char*>(&x)

#define GKC_ENSURE_FILE_OPEN(file)                          \
    do {                                                                \
        if (!(file).is_open()) {                                        \
            Debug::Logger::LogErrorWithType(ErrorType::FileNotOpen, "file is not open!");       \
            return;                                                    \
        }                                                               \
    } while (0)

#define GKC_ENSURE_FILE_OPEN_BOOL(file)                          \
    do {                                                                \
        if (!(file).is_open()) {                                        \
            Debug::Logger::LogErrorWithType(ErrorType::FileNotOpen, "file is not open!");       \
            return false;                                                    \
        }                                                               \
    } while (0)


#define GKC_PASSED_PARAM_NULL(param, paramType)             \
    if(param == nullptr) {                                  \
        GKC_ENGINE_INFO("{} passed is NULL!", paramType);   \
    }


#define GKC_EMPTY_STRING_RETURN(str, returnValue)                \
    do {                                        \
        if((str).empty()) {                     \
            ::Galaktic::Debug::Logger::LogErrorWithType(ErrorType::EmptyString, "'{}' is empty!", str); \
            return returnValue;         \
        }                       \
    } while(0)
