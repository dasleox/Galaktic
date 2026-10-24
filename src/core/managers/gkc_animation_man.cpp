#include <core/managers/gkc_animation_man.h>
#include <render/gkc_animation.h>
#include <filesys/gkc_filesys.h>
#include <core/gkc_logger.h>

using namespace Galaktic::Core;
using namespace Galaktic::Core::Managers;
using namespace Galaktic::Render;

static const vector<string> ANIMATION_EXTENSIONS = {".gif", ".apng"};

AnimationManager::AnimationManager(const string& folderPath) {
    auto files = Filesystem::GetFilenamesInFolder(folderPath);
    for (auto& file : files) {
        if (Filesystem::CheckExtension(file, ANIMATION_EXTENSIONS))
            RegisterAsset(file);
    }
}

void AnimationManager::UpdateAll(float deltaTime) {
    for (auto& [name, info] : m_assetList) {
        if (info != nullptr) {
            info->GetAsset()->Update(deltaTime);
        }
    }
}
void AnimationManager::PlayAnimation(const string& name)
{
    auto animation = GetAssetByName(name);
    if(animation == nullptr)
    {
        Debug::Logger::LogErrorWithType(ErrorType::AssetIsNull, "'{}' animation is NULL!", name);
        return;
    }

    animation->Play();
}

void AnimationManager::StopAnimation(const string& name)
{
    auto animation = GetAssetByName(name);
    if(animation == nullptr)
    {
        Debug::Logger::LogErrorWithType(ErrorType::AssetIsNull, "'{}' animation is NULL!", name);
        return;
    }

    animation->Stop();
}

void AnimationManager::PauseAnimation(const string& name)
{
    auto animation = GetAssetByName(name);
    if(animation == nullptr)
    {
        Debug::Logger::LogErrorWithType(ErrorType::AssetIsNull, "'{}' animation is NULL!", name);
        return;
    }

    animation->Pause();
}
void AnimationManager::SetLoopToAnimation(const string& name)
{
    auto animation = GetAssetByName(name);
    if(animation == nullptr)
    {
        Debug::Logger::LogErrorWithType(ErrorType::AssetIsNull, "'{}' animation is NULL!", name);
        return;
    }

    animation->SetLoop(true);
}