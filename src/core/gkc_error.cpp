#include <core/gkc_error.h>
#include <core/gkc_main.h>
#include <core/gkc_logger.h>
#include <pch.hpp>

using namespace Galaktic::Core;

Galaktic::Core::GalakticException::GalakticException(ErrorType type, const std::string& message)
        : std::runtime_error("[" + ErrorTypeToString(type) + "] " + message) {
    switch (type) {
        case ErrorType::InitFailure:
            GKC_ENGINE_FATAL(ErrorTypeToString(type) + ":" + message);
            break;
        case ErrorType::NotImplemented:
            GKC_ENGINE_WARNING(ErrorTypeToString(type) + ":" + message);
            break;
        case ErrorType::InvalidArgument:
            GKC_ENGINE_ERROR(ErrorTypeToString(type) + ":" + message);
            break;
        case ErrorType::OutOfMemory:
            GKC_ENGINE_FATAL(ErrorTypeToString(type) + ":" + message);
        default:
            break;
    }
}

void Galaktic::Core::InitLibraries() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        throw GalakticException(ErrorType::InitFailure, "Failed to initialize SDL: " + string(SDL_GetError()));
    }
}

std::string Galaktic::Core::ErrorTypeToString(ErrorType error) {
    switch (error) {

            // General
            case ErrorType::None:                 return "None";
            case ErrorType::Unknown:              return "Unknown";
            case ErrorType::NotImplemented:       return "Not Implemented";
            case ErrorType::InvalidArgument:      return "Invalid Argument";
            case ErrorType::NullPointer:          return "Null Pointer";
            case ErrorType::OutOfMemory:          return "Out of Memory";
            case ErrorType::Timeout:              return "Timeout";
            case ErrorType::OperationFailed:      return "Operation Failed";

            // Core / Galaktic
            case ErrorType::DependencyFailure:    return "Dependency Failure";
            case ErrorType::InitFailure:          return "Initialization Failure";
            case ErrorType::ShutdownFailure:      return "Shutdown Failure";
            case ErrorType::ConfigError:          return "Configuration Error";
            case ErrorType::VersionMismatch:      return "Version Mismatch";

            // Renderer
            case ErrorType::RendererFailure:         return "Renderer Failure";
            case ErrorType::InvalidTexture:          return "Invalid Texture";
            case ErrorType::TextureLoad:             return "Texture Load Failure";
            case ErrorType::TexureFormatUnsupported: return "Texture Format Unsupported";
            case ErrorType::BufferFailure:           return "Buffer Failure";
            case ErrorType::BufferOverflow:          return "Buffer Overflow";
            case ErrorType::GPUNotFound:             return "GPU Not Found";
            case ErrorType::GPUOutofMemory:          return "GPU Out of Memory";

            // Audio
            case ErrorType::AudioInitFaliure:        return "Audio Initialization Failure";
            case ErrorType::AudioDeviceNotFound:     return "Audio Device Not Found";
            case ErrorType::AudioFormatUnsupported:  return "Audio Format Unsupported";

            // Input
            case ErrorType::InputInitFailure:        return "Input Initialization Failure";
            case ErrorType::KeyboardNotFound:        return "Keyboard Not Found";
            case ErrorType::MouseNotFound:           return "Mouse Not Found";
            case ErrorType::InvalidKey:              return "Invalid Key";
            case ErrorType::InvalidMouseKey:         return "Invalid Mouse Key";

            // Files
            case ErrorType::InvalidDirectory:        return "Invalid Directory";
            case ErrorType::DirectoryNotFound:       return "Directory Not Found";
            case ErrorType::InvalidFile:             return "Invalid File";
            case ErrorType::FileNotFound:            return "File Not Found";
            case ErrorType::FilePermissionError:     return "File Permission Error";
            case ErrorType::DiskFull:                return "Disk Full";

            // Entity
            case ErrorType::EntityPathNotFound:      return "Entity Path Not Found";
            case ErrorType::InvalidEntity:           return "Invalid Entity";
            case ErrorType::EntityImportFailure:     return "Entity Import Failure";

            // Application
            case ErrorType::ApplicationNameExists:   return "Application Name Exists";
            case ErrorType::ApplicationCorrupted:    return "Application Corrupted";
            case ErrorType::ApplicationTerminated:   return "Application Terminated";

            // Scripting
            case ErrorType::ScriptNotValid:          return "Script Not Valid";
            case ErrorType::ScriptNotFound:          return "Script Not Found";
            case ErrorType::ScriptRuntimeError:      return "Script Runtime Error";
            default: break;           
    }
    return "Unrecognized ErrorType";    
}