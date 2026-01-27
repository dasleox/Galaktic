#include <core/managers/gkc_window_man.h>
#include "core/gkc_logger.h"
#include "render/gkc_window.h"

using namespace Galaktic::Core;

Managers::Window_List Managers::WindowManager::m_windowList;

void Managers::WindowManager::CreateGKCWindow(const string& title, Uint32 width,
    Uint32 height, Render::Window_Type type) {
    auto window = make_shared<Render::Window>(title, width, height, type);
    if (!window->IsValid()) {
        GKC_ENGINE_ERROR("Window couldn't be created correctly!");
        return;
    }

    GKC_WindowID id = m_windowList.size() + 1;
    window->SetWindowID(id);
    m_windowList.emplace(m_windowList.size() + 1, window);
}

void Managers::WindowManager::RegisterWindow(shared_ptr<Render::Window> window) {
    GKC_WindowID id = m_windowList.size() + 1;
    if (!window->IsValid()) {
        GKC_ENGINE_ERROR("Window passed is not valid!");
        return;
    }

    if (id + 1 > MAX_WINDOW_QUANTITY) {
        GKC_ENGINE_ERROR("Too many windows already exist!");
        return;
    }

    window->SetWindowID(id);
    m_windowList.emplace(id, window);
    GKC_ENGINE_INFO("'{0}' window (ID: {1}) has been added", id, window->GetTitle());
}

void Managers::WindowManager::UnregisterWindow(GKC_WindowID id) {
    GKC_ENGINE_INFO("'{0}' window (ID: {1}) has been deleted", id, GetWindow(id)->GetTitle());
    m_windowList.erase(id);
}

shared_ptr<Galaktic::Render::Window> Managers::WindowManager::GetWindow(GKC_WindowID id) {
    if (m_windowList.contains(id))
        return m_windowList[id];
    return nullptr;
}