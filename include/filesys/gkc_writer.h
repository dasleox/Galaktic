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

namespace Galaktic::Core {
    class Scene;
}
namespace Galaktic::ECS {
    class Entity;
    class Registry;
}

namespace Galaktic::Filesystem {
    /**
    * @class FileWriter
    * @brief Provides utility functions for writing various data types and engine objects to files.
    * 
    * @attention Many versions of reading or writing functions may be incompatible with different
    * verions of Galaktic. Please ensure that the version of the engine used to read a scene
    * matches the version used to write it.
    */
    class FileWriter {
        public:
            /**
             * @brief Writes a string to an output file stream.
             * @param file Output file stream to write to.
             * @param str String to write.
             */
            static void WriteString(ofstream& file, const string& str);

            /**
             * @brief Writes a generic data type to an output file stream.
             * @tparam T Type of the value to write.
             * @param file Output file stream to write to.
             * @param value Value to write.
             */
            template<typename T>
            static void Write(ofstream& file, const T& value) {
                file.write(reinterpret_cast<const char*>(&value), sizeof(T));
            }

            /**
             * Writes an entity to an output file stream, retrieving its components to the provided
             * entity and Registry. No checks are made to verify the integrity of the data,
             * if a scene has been tampered with, it may lead to crashes or undefined behavior.
             * 
             * @attention Many versions of reading or writing functions may be incompatible with different
             * verions of Galaktic. Please ensure that the version of the engine used to read a scene
             * matches the version used to write it.
             * @param file File stream to write to
             * @param entity Entity to write
             * @param registry Registry (ECS)
             */
            static void WriteEntity(ofstream& file, const ECS::Entity& entity,
                ECS::Registry* registry);

            /**
             * Writes the scene to a file path, adding its entities and components to the provided
             * ECS_Manager and Registry. No checks are made to verify the integrity of the data,
             * if a scene has been tampered with, it may lead to crashes or undefined behavior.
             * 
             * @attention Many versions of reading or writing functions may be incompatible with different
             * verions of Galaktic. Please ensure that the version of the engine used to read a scene
             * matches the version used to write it.
             * @param name Name of the file to output, (can be different from the scene name)
             * @param scene Scene reference to write data from
             * @param registry Registry (ECS)
             */
            static void WriteScene(const path& name, Core::Scene &scene, ECS::Registry *registry);

    };
}