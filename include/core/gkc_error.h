/*
  Galaktic Engine
  Copyright (C) 2025 Leonardo

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


// @todo Make an error catching function which calls another funciton to do something specific
//      like telling the engine to do an specific thing or display something like a message box
#ifndef GKC_ERROR_H
#define GKC_ERROR_H

#include <pch.hpp>
#include <core/gkc_main.h>

using std::string;

namespace Galaktic {
namespace Core {
    extern void InitLibraries();
    enum class ErrorType {
        // General  
        Unknown = -1,
        None,
        NotImplemented,
        InvalidArgument,
        NullPointer,
        OutOfMemory,
        Timeout,
        OperationFailed,

        // Core / Galaktic
        DependencyFailure,
        InitFailure,
        ShutdownFailure,
        ConfigError,
        VersionMismatch,

        // Renderer
        RendererFailure,
        InvalidTexture,
        TextureLoad,
        TexureFormatUnsupported,
        BufferFailure,
        BufferOverflow,
        GPUNotFound,
        GPUOutofMemory,
        
        // Audio
        AudioInitFaliure,
        AudioDeviceNotFound,
        AudioFormatUnsupported,
        
        // Input
        InputInitFailure,
        KeyboardNotFound,
        MouseNotFound,
        InvalidKey,
        InvalidMouseKey,

        // Files
        InvalidDirectory,
        DirectoryNotFound,
        InvalidFile,
        FileNotFound,
        FilePermissionError,
        DiskFull,
        
        // Entity
        EntityPathNotFound,
        InvalidEntity,
        EntityImportFailure,

        // Application
        ApplicationNameExists,
        ApplicationCorrupted,
        ApplicationTerminated,

        // Scripting
        ScriptNotValid,
        ScriptNotFound,
        ScriptRuntimeError
    };
    class GalakticException : public std::runtime_error {
        public:
            GalakticException(ErrorType type, const std::string& message);
    };

    std::string ErrorTypeToString(ErrorType error);
}
}

/*  Speaking of errors this is cool as fuck 
    https://www.youtube.com/watch?v=5BZLz21ZS_Y
*/

#endif