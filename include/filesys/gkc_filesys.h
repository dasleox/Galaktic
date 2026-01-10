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

/**
 * @brief Checks if the file exists, if it is returns(exits) the function using this macro
 * @param path file's path
 * @warning This macro only works if the function is of type void\n
 *          USING THIS IN A RETURN TYPE FUNCTION WILL CAUSE A COMPILER ERROR
 */
#define GKC_FILE_CHECK_RETURN(path) if (!Filesystem::CheckFile(path)) { GKC_CLIENT_ERROR("File doesn't exist!"); return; }

namespace Galaktic::Core {
	class Scene;
}

namespace Galaktic::Filesystem {
	/**
	 * @brief Checks if a directory exists in the given path.
	 * @param dir_path Directory path to check.
	 * @return true if the directory exists.
	 *         false if the directory doesn't exist (duh! x2)
	 */
	extern bool CheckDirectory(const path& dir_path);

	/**
	 * @brief Checks if a file exists at the given path.
	 * @param file_path File path to check.
	 * @return true if the file exists.
	 *         false if the file doesn't exist (duh! x3)
	 */
	extern bool CheckFile(const path& file_path);

	/**
	 * @brief Creates a new directory at the specified path.
	 * @param dir_path Directory path to create.
	 * @return true if the directory was created successfully.
	 *         false if creation failed or the directory already exists.
	 */
	extern bool CreateFolder(const path& dir_path);

	/**
	 * @brief Renames or moves a file from one path to another.
	 * @param old_path Current file path.
	 * @param new_path New file path (including new name if desired).
	 */
	extern void RenameFile(const path& old_path, const path& new_path);

	/**
	 * @brief Reads and lists the contents of a given folder.
	 * @param folder_path Folder path to read.
	 * @note This function prints folder files/entries
	 */
	extern void ReadFolderContent(const path& folder_path);

	/**
	 * @brief Reads information (e.g., size, modification date) about a specific file.
	 * @param file_path File path to read information from.
	 */
	extern void ReadFileInfo(const path& file_path);

	/**
	 * @brief Creates the directory structure for an app
	 * @param project_path File path to read information from.
	 */
	extern void CreateAppDirectoryStructure(const path& project_path);

	extern vector<string> GetFilenamesInFolder(const path& folder);

	extern void RefreshFolderContents(vector<string> &content, const path &folder);

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
	};

	/**
	 * @class FileWriter
	 * @brief Provides utility functions for writing various data types and engine objects to files.
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
	};

	const inline vector<path> APP_DIRECTORY_STRUCTURE = {
		"assets","scenes",
		"build"
	};
}

