#include <iostream>
#include <Galaktic.h>

int main()
{
    Galaktic::Debug::PrintEngineInformation();
    Galaktic::Debug::Logger::Init();

    GKC_ENGINE_FATAL("M83-Midnight City is needed bitch!");
    GKC_CLIENT_ERROR("You need a better dick bruh!");
    
    return 0;
}