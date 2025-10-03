#include <core/gkc_main.h>
#include <pch.hpp>

using std::cout, std::endl;

void Galaktic::Debug::PrintEngineInformation(){
    cout << "Galaktic Version: " << "Galaktic " << GKC_VERSION
         << " \"" << GKC_SUFFIX << "\" " << "Build " << GKC_BUILD_VERSION << endl;
}