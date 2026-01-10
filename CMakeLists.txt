cmake_minimum_required(VERSION 4.0)
project(Galaktic VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_PREFIX_PATH "C:/msys64/mingw64" ${CMAKE_PREFIX_PATH})
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" ON)

# ===========================
# Dependencies
# ===========================
find_package(Freetype REQUIRED)
find_package(SDL3 CONFIG REQUIRED)
find_package(SDL3_image CONFIG REQUIRED)
find_package(SDL3_ttf CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

# ===========================
# Engine
# ===========================
file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS src/*.cpp)
if(NOT ENGINE_SOURCES)
    message(FATAL_ERROR "No sources were found for Galaktic!")
endif()

add_library(Galaktic STATIC ${ENGINE_SOURCES})
target_include_directories(Galaktic PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_definitions(Galaktic
        PRIVATE
            GKC_ENABLE_ASSERTS=$<CONFIG:DEBUG>
            GKC_DEBUG=$<CONFIG:DEBUG>
            GKC_EVENT_DEBUG=$<CONFIG:DEBUG>
            GKC_PRINT_ADDRESSES=$<CONFIG:DEBUG>
)

##
if(ENABLE_UBSAN)
   # target_compile_options(Galaktic PRIVATE -fsanitize=undefined)
   # target_link_options(Galaktic PRIVATE -fsanitize=undefined)
endif ()
#
target_precompile_headers(Galaktic PRIVATE include/pch.hpp)

target_link_libraries(Galaktic
        PUBLIC
        SDL3::SDL3
        SDL3_image::SDL3_image
        SDL3_ttf::SDL3_ttf
        spdlog::spdlog
        nlohmann_json::nlohmann_json
)

# ===========================
# Platform-specific
# ===========================
if(UNIX AND NOT APPLE AND NOT MINGW)
    target_link_libraries(Galaktic
            PUBLIC
            x11
            dl
            pthread
    )
endif()

# ===========================
# Sandbox
# ===========================
file(GLOB_RECURSE SANDBOX_SOURCES CONFIGURE_DEPENDS examples/sandbox/*.cpp)

if(SANDBOX_SOURCES)
    add_executable(GalakticSandbox ${SANDBOX_SOURCES})
    target_link_libraries(GalakticSandbox PRIVATE Galaktic)

    set_target_properties(GalakticSandbox PROPERTIES
            RUNTIME_OUTPUT_DIR ${CMAKE_SOURCE_DIR}/bin
    )

    if(WIN32)
        message(STATUS "Ensure DLLs are next to the executable file")
    else()
        message(STATUS "Ensure shared libraries are next to the executable file")
    endif()
else()
    message(WARNING "No sources were found for Sandbox")
endif()