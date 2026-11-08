#include <audio/gkc_audio_group.h>
#include <core/managers/gkc_audio_man.h>
#include <core/gkc_logger.h>

using namespace Galaktic::Audio;
using namespace Galaktic::Core;

AudioGroup::AudioGroup(float maxVolume) {
    SetMaxVolume(maxVolume);
    m_currentVolume = m_maxVolume;
}

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
            "audio with ID: '{}' not found in this AudioGroup!", id);
    }

    m_audioList.erase(id);
}

void AudioGroup::SetVolume(float volume)
{
    m_currentVolume = std::min(Managers::AudioManager::ClampVolume(volume), m_maxVolume);
    for (auto id : m_audioList)
        Managers::AudioManager::SetVolumeByID(id, m_currentVolume);
}

void AudioGroup::Mute()
{
    SetVolume(0.f);
}

void AudioGroup::SetMaxVolume(float volume)
{
    m_maxVolume = Managers::AudioManager::ClampVolume(volume);
    SetVolume(m_currentVolume);
}
