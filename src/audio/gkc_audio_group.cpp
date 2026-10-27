#include <audio/gkc_audio_group.h>
#include <core/gkc_logger.h>
#include <core/gkc_error.h>
#include <core/managers/gkc_audio_man.h>

using namespace Galaktic::Audio;
using namespace Galaktic::Core;
using namespace Galaktic;

void AudioGroup::AddAudioFromGroup(AudioID id)
{
    if (Managers::AudioManager::AssetExistsByID(id))
    {
        m_audioList.emplace(id);
    }
}

void AudioGroup::DeleteAudioFromGroup(AudioID id)
{
    auto it = m_audioList.find(id);
    if(it == m_audioList.end())
    {
        Debug::Logger::LogErrorWithType(ErrorType::AudioGroupElementNotFound,
            "audio with ID: '{}' not found in this AudioGroup!");
    }

    m_audioList.erase(id);
}

void AudioGroup::SetVolume(float volume)
{
    if(!(volume > m_maxVolume))
    {
        volume = Managers::AudioManager::ClampVolume(volume);
        for(auto id : m_audioList)
        {
            Managers::AudioManager::SetVolumeByID(id, volume);
        }
    }
}

void AudioGroup::Mute()
{
    for(auto id : m_audioList)
    {
        Managers::AudioManager::SetVolumeByID(id, 0.f);
    }
}

void AudioGroup::SetMaxVolume(float volume)
{
    m_maxVolume = volume; 
    SetVolume(volume); 
}