#include <Galaktic.h>

int main(int argc, char** argv) {
    path current_path = std::filesystem::current_path();
    Galaktic::Core::App app(current_path, "Galaxy");
    app.LoadSpecificScene("Main");
}