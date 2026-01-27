/*
  Galaktic Engine
  Copyright (C) 2025 SummerChip

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

namespace Galaktic::Debug {
    class GalakticException : public std::exception {
        public:
            GalakticException(const string& message, const string& file,
                const string& function, int line);

            [[nodiscard]] const char* what() const noexcept override {
                return m_what.c_str();
            }

            const string& GetMessage() { return m_message; }
            const string& GetFile() { return m_file; }
            const string& GetFunction() { return m_function; }
            [[nodiscard]] int GetLine() const { return m_line; }

        private:
            void BuildWhat();

            string m_message;
            string m_file;
            string m_function;
            string m_what;
            int m_line;
    };

    class FilesystemException : public GalakticException {
        using GalakticException::GalakticException;
    };

    class EventException : public GalakticException {
        using GalakticException::GalakticException;
    };

    class RenderException : public GalakticException {
        using GalakticException::GalakticException;
    };

    class AudioException : public GalakticException {
        using GalakticException::GalakticException;
    };

    class WritingException : public GalakticException {
        using GalakticException::GalakticException;
    };

    class ReadingException : public GalakticException {
        using GalakticException::GalakticException;
    };
}

/**
 * @brief Macro to throw an exception by type
 * @param type Type of the exception
 * @param msg Message to print (what() text)
 */
#define GKC_THROW_EXCEPTION(type, msg) throw type(msg, __FILE__, __FUNCTION__, __LINE__)