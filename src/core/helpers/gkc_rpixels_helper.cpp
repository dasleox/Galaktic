#include <core/helpers/gkc_rpixels_helper.h>
#include <core/managers/gkc_ecs_man.h>
#include <render/gkc_rpixels.h>

using namespace Galaktic;
using namespace Galaktic::Core::Managers;
using namespace Galaktic::Core;

#define GROUP_EXISTS(groupname)                                                     \
    do {                                                                            \
        if ((m_groupList).find((groupName)) == (m_groupList).end()) {               \
            GKC_ENGINE_ERROR("Group '{}' to delete doesn't exists!", groupName);    \
            return;                                                                 \
        }                                                                           \
    } while(0)

Helpers::RPixelsHelper::RPixelsHelper(ECS_Manager& manager)
    : m_ecsManager(manager) {}

void Helpers::RPixelsHelper::CreatePixelGroup(const string& groupName, EntityID id) {
    shared_ptr group = make_shared<Render::PixelColoredGroupInfo>(groupName, RED_COLOR, id, m_ecsManager);

    if(group == nullptr) {
        GKC_ENGINE_ERROR("Failed to create pixel group '{}'", groupName);
        return;
    }

    m_groupList.emplace(groupName, move(group));
    GKC_ENGINE_INFO("Created pixel group '{}'", groupName);
}

void Helpers::RPixelsHelper::DeletePixelGroup(const string& groupName) {
    GROUP_EXISTS(groupName);
    m_groupList.erase(groupName);
}

void Helpers::RPixelsHelper::AddEntityToGroup(const string& groupName, EntityID id) {
    GROUP_EXISTS(groupName);
    auto group = m_groupList.find(groupName)->second;
    if(group == nullptr) {
        GKC_ENGINE_ERROR("Group '{}'is NULL!", groupName);
        return;
    }

    group->AddEntityToGroup(id);
}

void Helpers::RPixelsHelper::DeleteEntityFromGroup(const string& groupName, EntityID id) {
    GROUP_EXISTS(groupName);
    auto group = m_groupList.find(groupName)->second;
    if(group == nullptr) {
        GKC_ENGINE_ERROR("Group '{}'is NULL!", groupName);
        return;
    }

    group->DeleteEntityFromGroup(id);
}

void Helpers::RPixelsHelper::DrawAllGroups() {
    for(auto& [name , group] : m_groupList) {
        if(group == nullptr) 
            continue;
        
        group->ApplyColorToEntities();
    }
}


#undef GROUP_EXISTS