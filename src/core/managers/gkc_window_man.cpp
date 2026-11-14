#include <core/managers/gkc_window_man.h>
#include "core/gkc_logger.h"
#include "render/gkc_window.h"

using std::make_shared;
using std::shared_ptr;
using std::string;

using namespace Galaktic::Core;

Managers::Window_List Managers::WindowManager::m_windowList;

namespace {
    GKC_WindowID FindAvailableWindowID(const Managers::Window_List& windows) {
        for(GKC_WindowID id = 1; id <= MAX_WINDOW_QUANTITY; ++id) {
            if(!windows.contains(id)) return id;
        }
        return 0;
    }
}

bool Managers::WindowManager::CreateGKCWindow(const string& title, Uint32 width,
    Uint32 height, Render::Window_Type type) {
    const auto id = FindAvailableWindowID(m_windowList);
    if(id == 0) {
        GKC_ENGINE_ERROR("Too many windows already exist!");
        return false;
    }
    auto window = make_shared<Render::Window>(title, width, height, type);
    if (!window->IsValid()) {
        GKC_ENGINE_ERROR("Window couldn't be created correctly!");
        return false;
    }

    if(!m_windowList.emplace(id, window).second) return false;
    window->SetWindowID(id);
    return true;
}

void Managers::WindowManager::RegisterWindow(shared_ptr<Render::Window> window) {
    GKC_PASSED_PARAM_NULL(window, "Window");
    
    if (!window->IsValid()) {
        GKC_ENGINE_ERROR("Window passed is not valid!");
        return;
    }

    const auto id = FindAvailableWindowID(m_windowList);
    if (id == 0) {
        GKC_ENGINE_ERROR("Too many windows already exist!");
        return;
    }

    if(!m_windowList.emplace(id, window).second) return;
    window->SetWindowID(id);
    GKC_ENGINE_INFO("'{0}' window (ID: {1}) has been added", window->GetTitle(), id);
}

void Managers::WindowManager::UnregisterWindow(GKC_WindowID id) {
    const auto it = m_windowList.find(id);
    if(it == m_windowList.end()) {
        GKC_ENGINE_WARNING("Window with ID: {} to unregister doesn't exist!", id);
        return;
    }
    GKC_ENGINE_INFO("'{0}' window (ID: {1}) has been deleted", it->second->GetTitle(), id);
    m_windowList.erase(it);
}

shared_ptr<Galaktic::Render::Window> Managers::WindowManager::GetWindow(GKC_WindowID id) {
    if (m_windowList.contains(id)) {
        return m_windowList[id];
    }
    GKC_ENGINE_WARNING("Window with ID: {0} to retrieve doesn't exist!", id);
    return nullptr;
}
