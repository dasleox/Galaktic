/*
  Galaktic Engine
  Copyright (C) 2026 SummerChip

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#pragma once
#include <pch.hpp>

namespace Galaktic
{
    enum class ErrorType 
    {
        // General
        Unknown = 0,
        OutOfRange,
        NullPointer,
        IntegerOverflow,
        IntegerUnderflow,
        DivisionByZero,
        LibraryError,
        EmptyString,
        InvalidVersionMatch,

        // Memory
        AllocationFailed,
        DoubleFree,
        PoolOverflow,

        // Assets
        AssetNotFound,
        AssetIsNull,
        AssetNotLoaded,
        AssetUnloadError,
        AssetLoadError,
        AssetLimitReached,
        AssetInvalid,
        AssetRegistrationError,
        
        // Entity
        InvalidComponent,
        InvalidComponentCombination,
        InvalidEntity,
        EntityIsNull,
        EntityNotFound,
        EntityToFollowInvalid,
        EntityLimitReached,
        CameraNotSet,

        // Scene
        SceneNotFound,
        SceneLoadError,
        SceneSaveError,

        // Script
        ScriptNotFound,
        ScriptCompileFailed,
        ScriptRuntimeError,

        // Filesystem
        FileNotFound,
        FileNotOpen,
        FileCorrupted,
        FileReadError,
        DirectoryNotFound,
        DirectoryCreateError,
        PermissionDenied,
        RenameError,

        // Save and Read
        SaveFailed,
        ReadFailed,
        InvalidFileVersion,

        // Configuration
        BindNotFound,
        InvalidKey,
        InvalidKeyCombination,
        NoConfigFileFound,
        CallbackNotFound,
        InvalidCallback,

        // Dependencies
        DependencyNotFound,
        RenderError,
        WindowError,
        WritingError,
        ReadingError
    };

    constexpr const char* ErrorTypeToString(ErrorType error)
    {
        switch (error)
        {
            // General
            case ErrorType::Unknown:                    return "Unknown";
            case ErrorType::OutOfRange:                 return "Out Of Range";
            case ErrorType::NullPointer:                return "Null Pointer";
            case ErrorType::IntegerOverflow:            return "Integer Overflow";
            case ErrorType::IntegerUnderflow:           return "Integer Underflow";
            case ErrorType::EmptyString:                return "Empty String";
            case ErrorType::DivisionByZero:             return "Division By Zero";
            case ErrorType::LibraryError:               return "Library Error";
            case ErrorType::InvalidVersionMatch:        return "Invalid Version Match";

            // Memory
            case ErrorType::AllocationFailed:           return "Allocation Failed";
            case ErrorType::DoubleFree:                 return "Double Free";
            case ErrorType::PoolOverflow:               return "Pool Overflow";

            // Assets
            case ErrorType::AssetNotFound:              return "Asset Not Found";
            case ErrorType::AssetIsNull:                return "Asset Is Null";
            case ErrorType::AssetRegistrationError:     return "Asset Registration Error";
            case ErrorType::AssetNotLoaded:             return "Asset Not Loaded";
            case ErrorType::AssetUnloadError:           return "Asset Unload Error";
            case ErrorType::AssetLoadError:             return "Asset Load Error";
            case ErrorType::AssetInvalid:              return "Asset Invalid";
            case ErrorType::AssetLimitReached:          return "Asset Limit Reached";

            // Entity
            case ErrorType::InvalidComponent:           return "Invalid Component";
            case ErrorType::InvalidComponentCombination:return "Invalid Component Combination";
            case ErrorType::InvalidEntity:              return "Invalid Entity";
            case ErrorType::EntityIsNull:               return "Entity Is Null";
            case ErrorType::EntityNotFound:             return "Entity Not Found";
            case ErrorType::EntityToFollowInvalid:      return "Entity To Follow Invalid";
            case ErrorType::EntityLimitReached:         return "Entity Limit Reached";
            case ErrorType::CameraNotSet:               return "Camera Not Set";

            // Scene
            case ErrorType::SceneNotFound:              return "Scene Not Found";
            case ErrorType::SceneLoadError:             return "Scene Load Error";
            case ErrorType::SceneSaveError:             return "Scene Save Error";

            // Script
            case ErrorType::ScriptNotFound:             return "Script Not Found";
            case ErrorType::ScriptCompileFailed:        return "Script Compile Failed";
            case ErrorType::ScriptRuntimeError:         return "Script Runtime Error";

            // Filesystem
            case ErrorType::FileNotFound:               return "File Not Found";
            case ErrorType::FileNotOpen:                return "File Not Open";
            case ErrorType::FileCorrupted:              return "File Corrupted";
            case ErrorType::FileReadError:              return "File Read Error";
            case ErrorType::DirectoryNotFound:          return "Directory Not Found";
            case ErrorType::DirectoryCreateError:       return "Directory Create Error";
            case ErrorType::PermissionDenied:           return "Permission Denied";
            case ErrorType::RenameError:                return "Rename Error";

            // Save and Read
            case ErrorType::SaveFailed:                 return "Save Failed";
            case ErrorType::ReadFailed:                 return "Read Failed";
            case ErrorType::InvalidFileVersion:         return "Invalid File Version";

            // Configuration
            case ErrorType::BindNotFound:               return "Bind Not Found";
            case ErrorType::InvalidKey:                 return "Invalid Key";
            case ErrorType::InvalidKeyCombination:      return "Invalid Key Combination";
            case ErrorType::NoConfigFileFound:          return "No Config File Found";
            case ErrorType::InvalidCallback:            return "Invalid Callback";
            case ErrorType::CallbackNotFound:           return "Callback Not Found";

            // Dependencies
            case ErrorType::DependencyNotFound:         return "Dependency Not Found";
            case ErrorType::RenderError:                return "Render Error";
            case ErrorType::WindowError:                return "Window Error";
            case ErrorType::WritingError:               return "Writing Error";
            case ErrorType::ReadingError:                return "Reading Error";

            default:                                    return "Unknown Error";
        }
    }
    constexpr const char* ErrorTypeToDisplayString(ErrorType error)
    {
        switch (error)
        {
            // General
            case ErrorType::Unknown:                     return "\033[31m [Unknown] \033[0m";
            case ErrorType::OutOfRange:                  return "\033[31m [Out Of Range] \033[0m";
            case ErrorType::NullPointer:                 return "\033[31m [Null Pointer] \033[0m";
            case ErrorType::IntegerOverflow:             return "\033[31m [Integer Overflow] \033[0m";
            case ErrorType::IntegerUnderflow:            return "\033[31m [Integer Underflow] \033[0m";
            case ErrorType::EmptyString:                 return "\033[31m [Empty String] \033[0m";
            case ErrorType::DivisionByZero:              return "\033[31m [Division By Zero] \033[0m";
            case ErrorType::LibraryError:                return "\033[31m [Library Error] \033[0m";
            case ErrorType::InvalidVersionMatch:         return "\033[31m [Invalid Version Match] \033[0m";

            // Memory
            case ErrorType::AllocationFailed:            return "\033[31m [Allocation Failed] \033[0m";
            case ErrorType::DoubleFree:                  return "\033[31m [Double Free] \033[0m";
            case ErrorType::PoolOverflow:                return "\033[31m [Pool Overflow] \033[0m";

            // Assets
            case ErrorType::AssetNotFound:               return "\033[31m [Asset Not Found] \033[0m";
            case ErrorType::AssetIsNull:                 return "\033[31m [Asset Is Null] \033[0m";
            case ErrorType::AssetLimitReached:           return "\033[31m [Asset Limit Reached] \033[0m";
            case ErrorType::AssetRegistrationError:      return "\033[31m [Asset Registration Error] \033[0m";
            case ErrorType::AssetNotLoaded:              return "\033[31m [Asset Not Loaded] \033[0m";
            case ErrorType::AssetUnloadError:            return "\033[31m [Asset Unload Error] \033[0m";
            case ErrorType::AssetLoadError:              return "\033[31m [Asset Load Error] \033[0m";
            case ErrorType::AssetInvalid:                return "\033[31m [Asset Invalid] \033[0m";

            // Entity
            case ErrorType::InvalidComponent:            return "\033[31m [Invalid Component] \033[0m";
            case ErrorType::InvalidComponentCombination: return "\033[31m [Invalid Component Combination] \033[0m";
            case ErrorType::InvalidEntity:               return "\033[31m [Invalid Entity] \033[0m";
            case ErrorType::EntityIsNull:                return "\033[31m [Entity Is Null] \033[0";
            case ErrorType::EntityNotFound:              return "\033[31m [Entity Not Found] \033[0";
            case ErrorType::EntityToFollowInvalid:       return "\033[31m [Entity To Follow Invalid] \033[0m";
            case ErrorType::EntityLimitReached:          return "\033[31m [Entity Limit Reached] \033[0m";
            case ErrorType::CameraNotSet:                return "\033[31m [Camera Not Set] \033[0m";

            // Scene
            case ErrorType::SceneNotFound:               return "\033[31m [Scene Not Found] \033[0m";
            case ErrorType::SceneLoadError:              return "\033[31m [Scene Load Error] \033[0m";
            case ErrorType::SceneSaveError:              return "\033[31m [Scene Save Error] \033[0m";

            // Script
            case ErrorType::ScriptNotFound:              return "\033[31m [Script Not Found] \033[0m";
            case ErrorType::ScriptCompileFailed:         return "\033[31m [Script Compile Failed] \033[0m";
            case ErrorType::ScriptRuntimeError:          return "\033[31m [Script Runtime Error] \033[0m";

            // Filesystem
            case ErrorType::FileNotFound:                return "\033[31m [File Not Found] \033[0m";
            case ErrorType::FileNotOpen:                 return "\033[31m [File Not Open] \033[0m";
            case ErrorType::FileCorrupted:               return "\033[31m [File Corrupted] \033[0m";
            case ErrorType::FileReadError:               return "\033[31m [File Read Error] \033[0m";
            case ErrorType::DirectoryNotFound:           return "\033[31m [Directory Not Found] \033[0m";
            case ErrorType::DirectoryCreateError:        return "\033[31m [Directory Create Error] \033[0m";
            case ErrorType::PermissionDenied:            return "\033[31m [Permission Denied] \033[0m";
            case ErrorType::RenameError:                 return "\033[31m [Rename Error] \033[0m";

            // Save and Read
            case ErrorType::SaveFailed:                  return "\033[31m [Save Failed] \033[0m";
            case ErrorType::ReadFailed:                  return "\033[31m [Read Failed] \033[0m";
            case ErrorType::InvalidFileVersion:          return "\033[31m [Invalid File Version] \033[0m";

            // Configuration
            case ErrorType::BindNotFound:                return "\033[31m [Bind Not Found] \033[0m";
            case ErrorType::InvalidKey:                  return "\033[31m [Invalid Key] \033[0m";
            case ErrorType::InvalidKeyCombination:       return "\033[31m [Invalid Key Combination] \033[0m";
            case ErrorType::NoConfigFileFound:           return "\033[31m [No Config File Found] \033[0m";
            case ErrorType::InvalidCallback:             return "\033[31m [Invalid Callback] \033[0m";
            case ErrorType::CallbackNotFound:            return "\033[31m [Callback Not Found] \033[0m";

            // Dependencies
            case ErrorType::DependencyNotFound:          return "\033[31m [Dependency Not Found] \033[0m";
            case ErrorType::RenderError:                 return "\033[31m [Render Error] \033[0m";
            case ErrorType::WindowError:                 return "\033[31m [Window Error] \033[0m";
            case ErrorType::WritingError:                return "\033[31m [Writing Error] \033[0m";
            case ErrorType::ReadingError:                return "\033[31m [Reading Error] \033[0m";

            default:                                     return "\033[31m [Unknown Error] \033[0m";
        }
    }
}