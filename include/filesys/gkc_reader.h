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

namespace Galaktic::Core::Managers {
    class ECS_Manager;
}

namespace Galaktic::Core {
    class Scene;
}
namespace Galaktic::ECS {
    class Entity;
    class Registry;
}

namespace Galaktic::Filesystem {
    /**
    * @class FileReader
    * @brief Provides utility functions for reading various data types and engine objects from files.
    */
    class FileReader {
        public:
            /**
             * @brief Reads a string from an input file stream.
             * @param file Input file stream to read from.
             * @param str String reference to store the result.
             */
            static void ReadString(ifstream& file, string& str);

            /**
             * @brief Reads a generic data type from an input file stream.
             * @tparam T Type of the value to read.
             * @param file Input file stream to read from.
             * @param value Variable to store the read value.
             */
            template<typename T>
            static void Read(ifstream& file, T& value) {
                file.read(reinterpret_cast<char*>(&value), sizeof(T));
            }

            static void ReadEntity(ifstream& file, Core::Managers::ECS_Manager& manager,
                ECS::Registry* registry);

            static void ReadScene(const path& path, Core::Managers::ECS_Manager& manager,
                ECS::Registry* registry, Core::Scene& scene);
    };
}
