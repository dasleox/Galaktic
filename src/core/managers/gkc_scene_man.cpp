#include <core/managers/gkc_scene_man.h>
#include <filesys/gkc_filesys.h>
#include "core/gkc_logger.h"
#include "core/gkc_scene.h"
#include "filesys/gkc_reader.h"

using namespace Galaktic::Core;

Managers::SceneManager::SceneManager(const path &folder, ManagersWrapper* wrapper, const DeviceInformation &info)
    : m_deviceInfo(info), m_folder(folder), m_managersWrapper(wrapper) 
{
    GKC_ENGINE_INFO("SceneManager initialized with folder: {}", folder.string());
}

void Managers::SceneManager::CreateScene(const string &name) {
    m_sceneList.emplace(name, make_unique<Scene>(name, m_managersWrapper, m_deviceInfo, m_folder));
}   

void Managers::SceneManager::DeleteScene(const string &name) {
    auto it = m_sceneList.find(name);
    if (it != m_sceneList.end()) {
        it->second->Close();
        m_sceneList.erase(it);
    }
}

bool Managers::SceneManager::LoadSpecificScene(const string &name) const {
    if (!m_sceneList.contains(name)) {
        GKC_ENGINE_ERROR("Scene not found!");
        return false;
    }
    m_sceneList.find(name)->second->Run();
    return true;
}

bool Managers::SceneManager::LoadSpecificSceneFromFile(const path &filepath) {
    auto scenePath = m_folder / GKC_SCENE_PATH /filepath;

    #if GKC_DEBUG
        GKC_ENGINE_INFO("Loading scene from file: {}", scenePath.string());
    #endif
    
    if (!Filesystem::CheckFile(scenePath)) {
        GKC_ENGINE_ERROR("Scene not found in the specified filepath!");
        return false;
    }

    string sceneName = StripExtension(filepath.filename().string());
    auto m_scene = make_unique<Scene>(sceneName, m_managersWrapper, m_deviceInfo, m_folder);
    Filesystem::FileReader::ReadScene(scenePath, *m_scene.get()->GetECSManager(),
        m_scene.get()->GetRegistry(), *m_scene);
    m_sceneList.emplace(sceneName, std::move(m_scene));
    
    if(!LoadSpecificScene(sceneName)) {
        GKC_ENGINE_ERROR("Failed to load scene: {}", sceneName);
        return false;
    }

    return true;
}

void Managers::SceneManager::ShowSceneList()
{
    for(auto& [name, scene] : m_sceneList)
    {
        uint64_t fileSize = Filesystem::GetFileSize(m_folder / "scenes" / (name + ".gkscene"));
        GKC_ENGINE_INFO("Scene '{}' (Size: {} MB)", name, Core::BytesToMegabytes(fileSize));
    }
}

unique_ptr<Scene> Managers::SceneManager::GetScene(const string &name) {
    auto it = m_sceneList.find(name);
    if (it != m_sceneList.end()) {
        return std::move(it->second);
    }
    GKC_ENGINE_WARNING("Scene not found!");
    return nullptr;
}

bool Managers::SceneManager::SceneExists(const string& name)
{
    return m_sceneList.contains(name);
}