#include <core/managers/gkc_scene_man.h>
#include <filesys/gkc_filesys.h>
#include "core/gkc_logger.h"
#include "core/gkc_scene.h"
#include "filesys/gkc_reader.h"

using namespace Galaktic::Core;

Managers::SceneManager::SceneManager(const path &folder, ManagersWrapper* wrapper, const DeviceInformation &info)
    : m_deviceInfo(info), m_folder(folder), m_managersWrapper(wrapper) {}

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

void Managers::SceneManager::LoadSpecificScene(const string &name) const {
    if (!m_sceneList.contains(name)) {
        GKC_ENGINE_ERROR("Scene not found!");
        return;
    }
    m_sceneList.find(name)->second->Run();
}

void Managers::SceneManager::LoadSpecificSceneFromFile(const path &filepath) {
    if (!Filesystem::CheckFile(filepath)) {
        GKC_ENGINE_ERROR("Scene not found in the specified filepath!");
        return;
    }
    string sceneName = StripExtension(filepath.filename().string());
    auto m_scene = make_unique<Scene>(sceneName, m_managersWrapper, m_deviceInfo, m_folder);
    Filesystem::FileReader::ReadScene(filepath, *m_scene.get()->GetECSManager(),
        m_scene.get()->GetRegistry(), *m_scene);
    m_sceneList.emplace(sceneName, std::move(m_scene));
    LoadSpecificScene(sceneName);
}

unique_ptr<Scene> Managers::SceneManager::GetScene(const string &name) {
    auto it = m_sceneList.find(name);
    if (it != m_sceneList.end()) {
        return std::move(it->second);
    }
    GKC_ENGINE_WARNING("Scene not found!");
    return nullptr;
}

