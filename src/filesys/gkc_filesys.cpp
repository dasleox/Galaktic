#include <pch.hpp>
#include <filesys/gkc_filesys.h>
#include <core/gkc_logger.h>
#include "core/gkc_exception.h"

using namespace Galaktic;
using namespace std::filesystem;

bool Filesystem::CheckDirectory(const path& dir_path) {
    return exists(dir_path) && is_directory(dir_path);
}
bool Filesystem::CheckFile(const path& file_path) {
    return exists(file_path) && is_regular_file(file_path);
}

bool Filesystem::CreateFolder(const path& dir_name) {
    try {
        if (create_directory(dir_name)) {
            GKC_ENGINE_INFO("Created directory: {}", dir_name.string());
            return true;
        }
    }
    catch (const filesystem_error& e) {
        GKC_ENGINE_ERROR("Error creating directory '{}': {}", dir_name.string(), e.what());
        return false;
    }
    return false;
}

void Filesystem::RenameFile(const path& old_name, const path& new_name) {
    if (!CheckFile(old_name)) {
        GKC_THROW_EXCEPTION(Debug::FilesystemException, "File to rename doesn't exist!");
    }
    try {
        rename(old_name, new_name);
        GKC_ENGINE_INFO("Renamed file from '{}' to '{}'", old_name.string(), new_name.string());
    }
    catch (const filesystem_error& e) {
        GKC_ENGINE_ERROR("Error renaming file from '{}' to '{}': {}", old_name.string(), new_name.string(), e.what());
    }
}

void Filesystem::ReadFolderContent(const path& folder_path) {
    try {
        if(!CheckDirectory(folder_path)) {
            GKC_ENGINE_ERROR("The path '{}' is not a valid directory.", folder_path.string());
            return;
        }

        for(const auto& entry : directory_iterator(folder_path)) {
            GKC_ENGINE_INFO("Found: {}", entry.path().string());
        }
    }
    catch (const filesystem_error& e) {
        GKC_ENGINE_ERROR("Error reading directory '{}': {}", folder_path.string(), e.what());
    }
}

void Filesystem::ReadFileInfo(const path& file_path) {
    if(!CheckFile(file_path)) {
        GKC_ENGINE_ERROR("The path '{}' is not a valid file.", file_path.string());
        return;
    }

    auto fileSize = file_size(file_path);

    GKC_ENGINE_INFO("File: {0}", file_path.filename().string());
    GKC_ENGINE_INFO("Size: {0} bytes", fileSize);
}

void Filesystem::CreateAppDirectoryStructure(const path& project_path) {
    for(const auto& dir : APP_DIRECTORY_STRUCTURE) {
        path fullPath = path(project_path) / dir;
        if(!CheckDirectory(fullPath.string())) {
            CreateFolder(fullPath.string());
        }
    }
}

vector<string> Filesystem::GetFilenamesInFolder(const path &folder) {
    if (!CheckDirectory(folder)) {
        GKC_ENGINE_INFO("The path '{}' is not a valid directory.", folder.string());
        return {};
    }

    vector<string> fileNames;
    try {
        for (const auto& entry : recursive_directory_iterator(folder)) {
            if (entry.exists() && entry.is_regular_file())
                fileNames.push_back(entry.path().string());
        }
    } catch (const filesystem_error& e) {
        GKC_THROW_EXCEPTION(Debug::FilesystemException, e.what());
    }

    return fileNames;
}

void Filesystem::RefreshFolderContents(vector<string>& content, const path& folder) {
    content.empty() ? GKC_ENGINE_INFO("Passed folder content is empty!") : void(0);
    try {
        for (const auto& entry : recursive_directory_iterator(folder)) {
            if (entry.exists() && entry.is_regular_file())
                content.push_back(entry.path().string());
        }
    } catch (const filesystem_error& e) {
        GKC_THROW_EXCEPTION(Debug::FilesystemException, e.what());
    }
}

/* File Writer class */
void Filesystem::FileWriter::WriteString(ofstream& file, const string& str) {
    Uint32 length = str.size();
    file.write(reinterpret_cast<const char*>(&length), sizeof(length) );
    file.write(str.c_str(), length);
}
/* File Reader class */

void Filesystem::FileReader::ReadString(ifstream& file, string& str) {
    Uint32 length;
    Read(file, length);
    str.resize(length);
    file.read(str.data(), length);
}