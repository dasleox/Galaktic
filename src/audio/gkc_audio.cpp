#include <audio/gkc_audio.h>
#include "core/gkc_logger.h"
#include <core/gkc_error.h>
#include "filesys/gkc_filesys.h"

using namespace Galaktic;

Audio::AudioFile::AudioFile(const path &filepath, MIX_Mixer* mixer) {
    m_audio = MIX_LoadAudio(mixer, filepath.string().c_str(), true);
    if (m_audio == nullptr) {
        Debug::Logger::LogErrorWithType(ErrorType::AssetLoadError, "failed to load audio!");
    }
}