cmake_minimum_required(VERSION 3.28)
project(Galaktic VERSION 0.2.1 LANGUAGES CXX)

list(APPEND CMAKE_PREFIX_PATH "C:/msys64/mingw64")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" ON)

find_package(Freetype REQUIRED)
find_package(SDL3 CONFIG REQUIRED)
find_package(SDL3_image CONFIG REQUIRED)
find_package(SDL3_ttf CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(SDL3_Mixer CONFIG REQUIRED)

file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS src/*.cpp)
if(NOT ENGINE_SOURCES)
    message(FATAL_ERROR "No sources were found for Galaktic!")
endif()

add_library(Galaktic SHARED ${ENGINE_SOURCES})
add_library(luacpp SHARED)

set(LUACPP_MODULES
    libs/luacpp/Source/LuaCpp.cppm
)

file(GLOB_RECURSE LUACPP_CPP CONFIGURE_DEPENDS
    libs/luacpp/Source/LuaContext.cpp
    libs/luacpp/Source/LuaMetaObject.cpp
    libs/luacpp/Source/LuaVersion.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/luacpp/Source/Engine/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/luacpp/Source/Registry/*.cpp
)

file(GLOB_RECURSE LUACPP_HEADERS CONFIGURE_DEPENDS
    libs/luacpp/Source/Lua.hpp
    libs/luacpp/Source/LuaCpp.hpp
    libs/luacpp/Source/LuaContext.hpp
    libs/luacpp/Source/LuaMetaObject.hpp
    libs/luacpp/Source/LuaVersion.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/luacpp/Source/Engine/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/libs/luacpp/Source/Registry/*.hpp
)

target_sources(luacpp
    PRIVATE
        ${LUACPP_CPP}
        ${LUACPP_HEADERS}

    PRIVATE FILE_SET CXX_MODULES
        BASE_DIRS libs/luacpp/Source
        FILES ${LUACPP_MODULES}
)

target_include_directories(luacpp PUBLIC include)
target_link_libraries(luacpp PUBLIC lua)

# ===================== Galaktic =====================

target_include_directories(Galaktic PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_definitions(Galaktic
    PUBLIC
        GKC_ENABLE_ASSERTS=$<CONFIG:DEBUG>
        GKC_DEBUG=$<CONFIG:DEBUG>
        GKC_EVENT_DEBUG=$<CONFIG:DEBUG>
        GKC_PRINT_ADDRESSES=$<CONFIG:DEBUG>
        GKC_PRINT_TEXTURE_ADDED=$<CONFIG:DEBUG>
)

target_compile_options(Galaktic PUBLIC -Wextra)

target_precompile_headers(Galaktic PRIVATE include/pch.hpp)

target_link_libraries(Galaktic
    PUBLIC
        SDL3::SDL3
        SDL3_image::SDL3_image
        SDL3_ttf::SDL3_ttf
        spdlog::spdlog
        fmt::fmt
        nlohmann_json::nlohmann_json
        SDL3_mixer::SDL3_mixer
        luacpp
        lua
)

if(UNIX AND NOT APPLE AND NOT MINGW)
    target_link_libraries(Galaktic PUBLIC x11 dl pthread)
endif()

# ===================== Sandbox =====================

file(GLOB_RECURSE SANDBOX_SOURCES CONFIGURE_DEPENDS examples/sandbox/*.cpp)

if(SANDBOX_SOURCES)
    add_executable(GalakticSandbox ${SANDBOX_SOURCES})
    target_link_libraries(GalakticSandbox PRIVATE Galaktic)

    set_target_properties(GalakticSandbox PROPERTIES
        RUNTIME_OUTPUT_DIR ${CMAKE_SOURCE_DIR}/bin
    )
else()
    message(WARNING "No sources were found for Sandbox")
endif()