#include <core/managers/gkc_config_manager.h>
#include <config/gkc_config_file.h>
#include <core/gkc_logger.h>
#include <core/gkc_error.h>
#include <filesys/gkc_filesys.h>

using std::ofstream;
using std::filesystem::path;
using std::string;

using namespace Galaktic;
using namespace Galaktic::Core::Managers;

std::unordered_map<string, Config::ConfigFile> ConfigManager::m_configFileMap;
path ConfigManager::m_folderPath;

bool ConfigManager::CreateAudioConfigFile()
{
    const path& filePath = m_folderPath / "audio.cfg";
    GKC_FILE_ALREADY_EXISTS_BOOL(filePath);

    ofstream file(filePath);
    GKC_ENSURE_FILE_OPEN_BOOL(file);

    file << "audio.cfg\n";
    file << "# Galaktic Engine " << GKC_VERSION_STR << "\n";
    file << "# Format: variable=value;\n";
    file << "audio_channels=2;\n";
    file << "audio_frequency=48000;\n";
    file << "audio_format=F32;\n";           
    file << "master_volume=1.0;\n";
    file << "# Here will be added audio groups configurations\n";

    return true;
}

bool ConfigManager::CreateBindConfigFile()
{
    const path filePath = m_folderPath /"bind.cfg";
    GKC_FILE_ALREADY_EXISTS_BOOL(filePath);

    ofstream file(filePath);
    GKC_ENSURE_FILE_OPEN_BOOL(file);

    file << "bind.cfg\n";
    file << "# Galaktic Engine " << GKC_VERSION_STR << "\n";
    file << "# Format: action=key;\n";
    file << "# Keys: a-z, 0-9, f1-f12, space, return, escape, tab, left, right, up, down,\n";
    file << "#       left_shift, left_ctrl, left_alt, keypad_0-9\n";
    file << "# Mouse: left_click, right_click, middle_click\n";
    file << "move_forward=w;\n";
    file << "move_backward=s;\n";
    file << "move_left=a;\n";
    file << "move_right=d;\n";
    file << "jump=space;\n";
    file << "crouch=left_ctrl;\n";
    file << "duck=c;\n";
    file << "attack=left_click;\n";
    file << "secondary_attack=right_click;\n";
    file << "special_attack=middle_click;\n";
    file << "# User defined actions (action0 - action9)\n";
    file << "toggle_fullscreen=f11;\n";
    file << "toggle_debug=f3;\n";
    return true;
}

bool ConfigManager::CreateGeneralConfigFile()
{
    const path filePath = m_folderPath / "general.cfg";
    GKC_FILE_ALREADY_EXISTS_BOOL(filePath);

    ofstream file(filePath);
    GKC_ENSURE_FILE_OPEN_BOOL(file);

    file << "general.cfg\n";
    file << "# Galaktic Engine " << GKC_VERSION_STR << "\n";
    file << "# Format: variable=value;\n";
    file << "# Booleans: true / false | Colors: 0xRRGGBBAA\n";

    file << "\n# ---------------- Window ----------------\n";
    file << "window_title=Galaktic;\n";
    file << "# Fullscreen, Borderless, Transparent, Resizable, Hidden, OpenGL\n";
    file << "window_type=Resizable;\n";
    file << "# 0 = use the display resolution\n";
    file << "window_width=0;\n";
    file << "window_height=0;\n";
    file << "window_fullscreen=false;\n";
    file << "window_clear_color=0xFFFFFFFF;\n";

    file << "\n# ---------------- Paths ----------------\n";
    file << "texture_path=" << GKC_TEXTURE_PATH.generic_string() << ";\n";
    file << "sound_path=" << GKC_SOUND_PATH.generic_string() << ";\n";
    file << "animation_path=" << GKC_ANIMATION_PATH.generic_string() << ";\n";
    file << "scene_path=" << GKC_SCENE_PATH.generic_string() << ";\n";
    file << "script_path=" << GKC_SCRIPT_PATH.generic_string() << ";\n";
    file << "font_path=assets/fonts;\n";
    file << "fallback_font_path=" << GKC_ARIAL_FONT.generic_string() << ";\n";

    file << "\n# ---------------- Physics ----------------\n";
    file << "gravity=-9.81;\n";
    file << "floor_height=0.0;\n";
    file << "use_floor=true;\n";

    file << "\n# ---------------- Camera ----------------\n";
    file << "camera_zoom=1.0;\n";
    file << "camera_smoothing=3.0;\n";

    file << "\n# ---------------- Debug ----------------\n";
    file << "show_wireframes=false;\n";
    file << "show_colliders=false;\n";
    file << "show_fps=false;\n";
    file << "show_debug_info=false;\n";
    file << "wireframe_color=0xFFFF00FF;\n";
    file << "collider_color=0x00FF00BF;\n";

    return true;
}

bool ConfigManager::SetFolder(const path& path)
{
    m_folderPath = path;
    CreateAudioConfigFile();
    CreateBindConfigFile();
    CreateGeneralConfigFile();
    
    auto files = Filesystem::GetFilenamesInFolder(path);
    if(files.empty())
    {
        Debug::Logger::LogErrorWithType(ErrorType::DirectoryEmpty, "{} is empty!", path.string());
        return false;
    }    
    for(auto& file : files)
    {
        if(!AddConfigFile(file))
            Debug::Logger::LogErrorWithType(ErrorType::FileReadError, "{} couldn't load configuration file",
                file);
    }

    return true;
}

bool ConfigManager::AddConfigFile(const path& path)
{
    GKC_CHECK_PATH_INTEGRITY_BOOL(path);
    auto configFile = Config::ConfigFile(path);
    return true;
}

Config::ConfigFile* ConfigManager::GetConfigFile(const std::string& filename)
{
    if(!m_configFileMap.contains(filename)) return nullptr;

    return &m_configFileMap.at(filename);
}

std::vector<std::string> ConfigManager::GetConfigFileNames()
{
    auto filenames = std::vector<std::string>();
    for(const auto& [filename, _] : m_configFileMap)
    {
        filenames.push_back(filename);
    } 

    return filenames;
}
