#include <core/managers/gkc_animation_man.h>
#include <render/gkc_animation.h>
#include <filesys/gkc_filesys.h>
#include <core/gkc_logger.h>

using namespace Galaktic::Core;
using namespace Galaktic::Core::Managers;
using namespace Galaktic::Render;

Galaktic::Render::Animation_List Managers::AnimationManager::m_animationList;
vector<path> Managers::AnimationManager::m_animationPathList;
Galaktic::Render::AnimationID_List Managers::AnimationManager::m_IDToNameList;

AnimationManager::AnimationManager(const string& folderPath) {
    auto files = Filesystem::GetFilenamesInFolder(folderPath);
    for (auto& file : files) {
        if (Render::CheckAnimationExtension(file))
            AddAnimationPath(file);
    }
}

void AnimationManager::AddAnimation(const string& filePath, SDL_Renderer* renderer) {
    AnimationID id = m_animationList.size() + 1;
    auto info = make_shared<Render::AnimationInfo>(id, make_shared<Render::Animation>(filePath, renderer));
    
    if (!info->animation_->IsValid() || info->id_ == 0) {
        GKC_ENGINE_ERROR("Failed to load animation at path: {0}", filePath);
        return;
    }
    
    auto [it, inserted] = m_animationList.emplace(Filesystem::GetFilename(filePath), std::move(info));
    m_IDToNameList.emplace(id, Filesystem::GetFilename(filePath));
    m_animationPathList.emplace_back(filePath);
    
    GKC_ENGINE_INFO("'{}' animation added and loaded successfully!", filePath);
}

void AnimationManager::AddAnimationPath(const string& filePath) {
    AnimationID id = m_animationList.size() + 1;
    auto info = make_shared<Render::AnimationInfo>(id, nullptr);
    
    if (info == nullptr || info->id_ == 0) {
        GKC_ENGINE_ERROR("Failed to add animation at path: {0}", filePath);
        return;
    }
    
    auto [it, inserted] = m_animationList.emplace(Filesystem::GetFilename(filePath), std::move(info));
    m_IDToNameList.emplace(id, Filesystem::GetFilename(filePath));
    m_animationPathList.emplace_back(filePath);
    
    GKC_ENGINE_INFO("'{}' animation added successfully!", filePath);
    GKC_ENGINE_INFO("REMINDER: '{}' has to be loaded before use", filePath);
}

void AnimationManager::LoadAnimation(const string& filePath, SDL_Renderer* renderer) {
    string animationName = Filesystem::GetFilename(filePath);
    auto it = m_animationList.find(animationName);
    
    if (it != m_animationList.end()) {
        AnimationID id = it->second->id_;
        auto animation = make_shared<Render::Animation>(filePath, renderer);
        if (animation == nullptr) {
            GKC_ENGINE_ERROR("Failed to load animation at path: {0}", filePath);
            return;
        } else {
            if(!animation->IsValid()) {
                GKC_ENGINE_ERROR("Failed to load animation at path: {0}", filePath);
                return;
            }
        }
        
        it->second->animation_ = std::move(animation);
        GKC_ENGINE_INFO("'{}' animation loaded successfully", animationName);
    } else {
        AddAnimation(filePath, renderer);
    }
}

void AnimationManager::LoadAllAnimations(SDL_Renderer* renderer) {
    for (auto& path : m_animationPathList) {
        LoadAnimation(path.string(), renderer);
    }
    PrintList();
}

void AnimationManager::DeleteAnimation(const string& name) {
    auto animation = m_animationList.find(name);
    if (animation != m_animationList.end()) {
        m_IDToNameList.erase(animation->second->id_);
        m_animationList.erase(animation);
        GKC_ENGINE_INFO("Erased {0}", name);
    }
}

shared_ptr<Animation> AnimationManager::GetAnimation(const string& name) {
    auto animation = m_animationList.find(name);
    if (animation != m_animationList.end()) {
        if (animation->second != nullptr && animation->second->animation_ != nullptr) {
            return animation->second->animation_;
        }
        GKC_ENGINE_WARNING("Animation key exists but hasn't been loaded yet!");
        return nullptr;
    }
    GKC_ENGINE_WARNING("Animation to retrieve doesn't exist!");
    return nullptr;
}

shared_ptr<Animation> AnimationManager::GetAnimation(AnimationID id) {
    auto nameToFind = m_IDToNameList.find(id);
    if (nameToFind != m_IDToNameList.end()) {
        return GetAnimation(nameToFind->second);
    }
    GKC_ENGINE_WARNING("Animation to retrieve doesn't exist");
    return nullptr;
}

shared_ptr<AnimationInfo> AnimationManager::GetAnimationInfo(const string& name) {
    auto it = m_animationList.find(name);
    if (it != m_animationList.end()) {
        if (it->second != nullptr) {
            return it->second;
        }
        GKC_ENGINE_WARNING("Animation Information is NULL!");
        return nullptr;
    }
    GKC_ENGINE_WARNING("Animation Information to retrieve doesn't exist");
    return nullptr;
}

void AnimationManager::UpdateAll(float deltaTime) {
    for (auto& [name, info] : m_animationList) {
        if (info && info->animation_) {
            info->animation_->Update(deltaTime);
        }
    }
}

void AnimationManager::PrintList() {
    for (auto& pair : m_animationList) {
        const void* animationAddress = TakeAddressOfAnimation(pair.second.get());
        
        if (animationAddress == nullptr) {
            GKC_ENGINE_INFO("Key: {0} | {1} Address: {2}", 
                pair.first, pair.second->id_, "nullptr (needs to be loaded)");
        } else {
            GKC_ENGINE_INFO("Key: {0} | {1} Address: {2}", 
                pair.first, pair.second->id_, animationAddress);
        }
    }
}

const void* AnimationManager::TakeAddressOfAnimation(const Render::AnimationInfo* animInfo) {
    if (animInfo != nullptr) {
        auto& animation = animInfo->animation_;
        if (animation != nullptr) {
            return static_cast<const void*>(animation.get());
        }
        return nullptr;
    }
    return nullptr;
}