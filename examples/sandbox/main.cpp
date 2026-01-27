#include <Galaktic.h>

int main(int argc, char** argv) {
    path current_path = std::filesystem::current_path();
    cout << current_path << endl;
    Galaktic::Core::App app(current_path, "Galaxy");
    app.GetSceneManager()->LoadSpecificSceneFromFile("Galaxy/scenes/Fulano.gkscene");
    return 0;
}