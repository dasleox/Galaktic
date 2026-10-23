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

#define CASE_ERROR_RETURN(name) case ErrorType::name: return "\033[31m [" #name "] \033[0m";

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
        
        // Entity Components
        InvalidComponent,
        InvalidComponentType,
        InvalidComponentCombination,
        ComponentNotFound,

        // Entity
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

    constexpr const char* ErrorTypeToDisplayString(ErrorType error)
    {
        switch (error)
        {
            // General
            CASE_ERROR_RETURN(OutOfRange)
            CASE_ERROR_RETURN(NullPointer)
            CASE_ERROR_RETURN(IntegerOverflow)
            CASE_ERROR_RETURN(IntegerUnderflow)
            CASE_ERROR_RETURN(EmptyString)
            CASE_ERROR_RETURN(DivisionByZero)
            CASE_ERROR_RETURN(LibraryError)
            CASE_ERROR_RETURN(InvalidVersionMatch)

            // Memory
            CASE_ERROR_RETURN(AllocationFailed)
            CASE_ERROR_RETURN(DoubleFree)
            CASE_ERROR_RETURN(PoolOverflow)

            // Assets
            CASE_ERROR_RETURN(AssetNotFound)
            CASE_ERROR_RETURN(AssetIsNull)
            CASE_ERROR_RETURN(AssetLimitReached)
            CASE_ERROR_RETURN(AssetRegistrationError)
            CASE_ERROR_RETURN(AssetNotLoaded)
            CASE_ERROR_RETURN(AssetUnloadError)
            CASE_ERROR_RETURN(AssetLoadError)
            CASE_ERROR_RETURN(AssetInvalid)

            // Entity Components
            CASE_ERROR_RETURN(InvalidComponent)
            CASE_ERROR_RETURN(InvalidComponentType)
            CASE_ERROR_RETURN(InvalidComponentCombination)
            CASE_ERROR_RETURN(ComponentNotFound)

            // Entity
            CASE_ERROR_RETURN(InvalidEntity)
            CASE_ERROR_RETURN(EntityIsNull)
            CASE_ERROR_RETURN(EntityNotFound)
            CASE_ERROR_RETURN(EntityToFollowInvalid)
            CASE_ERROR_RETURN(EntityLimitReached)
            CASE_ERROR_RETURN(CameraNotSet)

            // Scene
            CASE_ERROR_RETURN(SceneNotFound)
            CASE_ERROR_RETURN(SceneLoadError)
            CASE_ERROR_RETURN(SceneSaveError)

            // Script
            CASE_ERROR_RETURN(ScriptNotFound)
            CASE_ERROR_RETURN(ScriptCompileFailed)
            CASE_ERROR_RETURN(ScriptRuntimeError)

            // Filesystem
            CASE_ERROR_RETURN(FileNotFound)
            CASE_ERROR_RETURN(FileNotOpen)
            CASE_ERROR_RETURN(FileCorrupted)
            CASE_ERROR_RETURN(FileReadError)
            CASE_ERROR_RETURN(DirectoryNotFound)
            CASE_ERROR_RETURN(DirectoryCreateError)
            CASE_ERROR_RETURN(PermissionDenied)
            CASE_ERROR_RETURN(RenameError)

            // Save and Read
            CASE_ERROR_RETURN(SaveFailed)
            CASE_ERROR_RETURN(ReadFailed)
            CASE_ERROR_RETURN(InvalidFileVersion)

            // Configuration
            CASE_ERROR_RETURN(BindNotFound)
            CASE_ERROR_RETURN(InvalidKey)
            CASE_ERROR_RETURN(InvalidKeyCombination)
            CASE_ERROR_RETURN(NoConfigFileFound)
            CASE_ERROR_RETURN(InvalidCallback)
            CASE_ERROR_RETURN(CallbackNotFound)

            // Dependencies
            CASE_ERROR_RETURN(DependencyNotFound)
            CASE_ERROR_RETURN(RenderError)
            CASE_ERROR_RETURN(WindowError)
            CASE_ERROR_RETURN(WritingError)
            CASE_ERROR_RETURN(ReadingError)

            default: return "\033[31m [Unknown Error] \033[0m";

        }
    }
}

#undef CASE_ERROR_RETURN