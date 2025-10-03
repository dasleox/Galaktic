#include <iostream>
#include <Galaktic.h>

int main()
{
    Galaktic::Core::App* app = new Galaktic::Core::App("Nadaplete :,)", Galaktic::Core::AppType::Application);

    app->Run();

    delete app;

    return 0;
}