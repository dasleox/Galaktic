cmake_minimum_required(VERSION 3.20)
project(GalakticEngine VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ===========================
# Engine Library (.a) || Satanic, ehm Static!
# ===========================
file(GLOB_RECURSE ENGINE_SOURCES src/*.cpp)

if(NOT ENGINE_SOURCES)
    message(FATAL_ERROR "No sources found in src/ for GalakticEngine")
endif()

add_library(GalakticEngine STATIC ${ENGINE_SOURCES})

target_include_directories(GalakticEngine
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/spdlog/include
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/SDL/include
)

# SDL3 (static .a from libs/SDL/build)
set(SDL3_LIB ${CMAKE_CURRENT_SOURCE_DIR}/libs/SDL/build/libSDL3.a)

if(NOT EXISTS ${SDL3_LIB})
    message(FATAL_ERROR "SDL3 static library not found at: ${SDL3_LIB}")
endif()

target_link_libraries(GalakticEngine
    PRIVATE
        ${SDL3_LIB}
        pthread
        dl
        X11
)

# Put engine output in build/lib
set_target_properties(GalakticEngine PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

# ===========================
# Sandbox Executable
# ===========================
file(GLOB_RECURSE SANDBOX_SOURCES example/Sandbox/*.cpp)

if(SANDBOX_SOURCES)
    add_executable(GalakSandbox ${SANDBOX_SOURCES})
    target_link_libraries(GalakSandbox PRIVATE GalakticEngine)

    # Put executable in build/bin
    set_target_properties(GalakSandbox PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
else()
    message(WARNING "No sources found in example/Sandbox, skipping GalakSandbox build")
endif()
