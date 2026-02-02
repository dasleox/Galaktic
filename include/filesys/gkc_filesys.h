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

/**
 * @brief Checks if the file exists, if it is returns(exits) the function using this macro
 * @param path file's path
 * @warning This macro only works if the function is of type void\n
 *          USING THIS IN A RETURN TYPE FUNCTION WILL CAUSE A COMPILER ERROR
 */
#define GKC_FILE_CHECK_RETURN(path) if (!Filesystem::CheckFile(path)) { GKC_CLIENT_ERROR("File doesn't exist!"); return; }

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
	 * @brief Deletes a file at the specified path.
	 * @param filepath filepath
	 */
	extern void RemoveFile(const path& filepath);

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

	/**
	 * @brief Get the Filenames in a folder path
	 * @param folder folder path
	 * @return vector with filenames, empty vector if the folder doesn't exist 
	 * 		   or the folder is empty
	 */
	extern vector<string> GetFilenamesInFolder(const path& folder);

	/**
	 * @brief Extracts the filename from a full file path.
	 * @param fullPath Full file path.
	 * @return Filename as a string.
	 */
	extern string GetFilename(const path& fullPath);

	/**
	 * @brief Extracts the filename from a full file path relative to a given path.
	 * @param fullPath Full file path.
	 * @param relativePath Relative path to extract from.
	 * @return Filename as a string.
	 */
	extern string GetFilenameFromRelativePath(const path& fullPath, const path& relativePath);

	/**
	 * @brief Refreshes the contents of a folder and updates the provided vector with filenames.
	 * @param content vector string
	 * @param folder folder to scan (can be the same)
	 */
	extern void RefreshFolderContents(vector<string> &content, const path &folder);

	/**
	 * @brief Gets the full path of the application. (.exe location)
	 * @return Full application path
	 */
	extern path GetFullPath();

	const inline vector<path> APP_DIRECTORY_STRUCTURE = {
		"assets", "assets/textures", "assets/sounds",
		"scenes", "scripts"
	};
}

