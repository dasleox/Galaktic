#include <audio/gkc_audio.h>
#include "core/gkc_exception.h"
#include "core/gkc_logger.h"
#include "filesys/gkc_filesys.h"

using namespace Galaktic;

Audio::AudioFile::AudioFile(const path &filepath, MIX_Mixer* mixer) {
    GKC_ENGINE_INFO("Loading {0}...", filepath.string());
    if (filepath.empty() || !Filesystem::CheckFile(filepath)) {
        GKC_ENGINE_ERROR("given path doesn't exists!");
    }

    m_audio = MIX_LoadAudio(mixer, filepath.string().c_str(), true);
    if (m_audio == nullptr) {
        GKC_ENGINE_ERROR("failed to load audio!");
    }
}

bool Audio::AudioFile::IsValid() const {
    return m_audio != nullptr;
}

bool Audio::CheckAudioExtension(const path &path) {
    if (path.empty() || !Filesystem::CheckFile(path)) {
        return false;
    }
    auto pathStr = path.extension().string();

    if (pathStr == ".wav" || pathStr == ".mp3" || pathStr == ".mp2" || pathStr == ".aiff"
        || pathStr == ".aac" || pathStr == ".ogg") { return true; }

    return false;
}
