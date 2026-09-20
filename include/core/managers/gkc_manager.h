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
#include <core/gkc_logger.h>
#include <filesys/gkc_filesys.h>
#include <core/gkc_error.h>
#include <type_traits>

namespace Galaktic::Core::Managers
{
    template<class AssetType, typename AssetInfo, typename TypeList, uint32_t MaxLimit, bool isStatic = false>
    class BaseManager;

    /**
     * @class BaseManager
     *
     * Specialized partial template for asset management, handles registration,
     * loading, retrieval and deletion of assets. Assets are stored in a \c TypeList
     * container keyed by a \c uint32_t ID, while a secondary \c std::unordered_map
     * maps asset filenames to their corresponding IDs. \n
     *
     * Assets can be registered without loading (deferred loading) via \c RegisterAsset,
     * or registered and loaded in a single step via \c LoadAndRegisterAsset. \n
     * Already registered assets can be loaded at any time using \c LoadAsset or
     * all at once using \c LoadAllAssets. \n
     *
     * @attention Key values for the asset list are registered using only the filename
     * <b>(extension included!)</b>, do not use the full filepath when retrieving
     * assets by name, only the filename.
     *
     * @tparam AssetType  The underlying asset type being managed
     * @tparam AssetInfo  The info wrapper that holds the asset and its metadata
     * @tparam TypeList   The container type used for the asset list (keyed by \c uint32_t)
     * @tparam MaxLimit   Maximum number of assets allowed in the list
     */
    template<class AssetType, typename AssetInfo, typename TypeList, uint32_t MaxLimit>
    class BaseManager<AssetType, AssetInfo, TypeList, MaxLimit, true>
    {
        protected:
            static inline TypeList m_assetList;                          
            static inline unordered_map<string, uint32_t> m_NameToIDList;
            static inline unordered_map<uint32_t, string> m_IDToNameList;
            static inline std::atomic<uint32_t> m_IDCounter{1};
        public:
            /**
             * Registers an asset into the asset list without loading it,
             * useful for deferred loading. The asset can be loaded later
             * using \c LoadAsset or \c LoadAllAssets
             *
             * @param filepath Full path to the asset file
             *
             * @note Only the filename (extension included) is used as the key
             * in \c m_NameToIDList
             */
            static void RegisterAsset(const string& filepath)
            {
                if (m_assetList.size() >= MaxLimit)
                {
                    Debug::Logger::LogErrorWithType(ErrorType::AssetLimitReached,
                        "Maximum number of assets of type: '{}' reached! ({} / {})",
                        Debug::GetCleanTypename<AssetType>(), MaxLimit, m_assetList.size());
                    return;
                }

                GKC_CHECK_PATH_INTEGRITY(path(filepath));

                uint32_t id = m_IDCounter.fetch_add(1);
                auto assetInfo = make_unique<AssetInfo>(filepath);

                if (assetInfo == nullptr)
                {
                    Debug::Logger::LogErrorWithType(ErrorType::AssetRegistrationError,
                        "Failed to register asset '{}'", filepath);
                    m_IDCounter.fetch_sub(1);
                    return;
                }

                string assetName = Filesystem::GetFilename(filepath);

                m_assetList.emplace(id, std::move(assetInfo));
                m_NameToIDList.emplace(assetName, id);
                m_IDToNameList.emplace(id, assetName);

                GKC_ENGINE_INFO("'{}' registered successfully!", filepath);
            }

            /**
             * Registers and immediately loads an asset into the asset list
             * in a single step, any additional arguments required by the
             * asset's \c LoadAsset method can be forwarded
             *
             * @param filepath Full path to the asset file
             * @param args     Additional arguments forwarded to \c LoadAsset
             */
            template<typename... Args>
            static void LoadAndRegisterAsset(const string& filepath, Args&&... args)
            {
                if (m_assetList.size() >= MaxLimit)
                {
                    Debug::Logger::LogErrorWithType(ErrorType::AssetLimitReached,
                        "Maximum number of assets of type: '{}' reached! ({} / {})",
                        Debug::GetCleanTypename<AssetType>(), MaxLimit, m_assetList.size());
                    return;
                }

                GKC_CHECK_PATH_INTEGRITY(path(filepath));

                uint32_t id = m_IDCounter.fetch_add(1);
                auto assetInfo = make_unique<AssetInfo>(filepath);

                if (assetInfo == nullptr)
                {
                    Debug::Logger::LogErrorWithType(ErrorType::AssetRegistrationError,
                        "Failed to register asset '{}'", filepath);
                    m_IDCounter.fetch_sub(1);
                    return;
                }

                if (!assetInfo->LoadAsset(filepath, std::forward<Args>(args)...))
                {
                    Debug::Logger::LogErrorWithType(ErrorType::AssetLoadError,
                        "Failed to load '{}'", filepath); 
                    m_IDCounter.fetch_sub(1);
                    return;
                }

                string assetName = Filesystem::GetFilename(filepath);
                m_assetList.emplace(id, std::move(assetInfo));
                m_NameToIDList.emplace(assetName, id);
                m_IDToNameList.emplace(id, assetName);

                GKC_ENGINE_INFO("'{}' loaded and registered successfully!", filepath);
            }

             /**
             * Loads an already registered asset by its filepath, the asset
             * must have been previously registered via \c RegisterAsset,
             * any additional arguments required by the asset's \c LoadAsset
             * method can be forwarded
             *
             * @param filepath Full path to the asset file
             * @param args     Additional arguments forwarded to \c LoadAsset
             *
             */
            template<typename... Args>
            static void LoadAsset(const string& filepath, Args&&... args)
            {
                string assetName = Filesystem::GetFilename(filepath);

                if (!AssetExistsByName(assetName))
                {
                    Debug::Logger::LogErrorWithType(ErrorType::AssetNotFound,
                        "'{}' to load doesn't exist!", filepath);
                    return;
                }

                AssetInfo* assetInfo = GetAssetInfo(assetName);
                if (assetInfo == nullptr)
                {
                    Debug::Logger::LogErrorWithType(ErrorType::AssetIsNull,
                        "'{}' asset info is NULL!", filepath);
                    return;
                }

                if (!assetInfo->LoadAsset(filepath, std::forward<Args>(args)...))
                {
                    Debug::Logger::LogErrorWithType(ErrorType::AssetLoadError,
                        "Failed to load '{}'", filepath);
                    return;                              
                }

                GKC_ENGINE_INFO("'{}' loaded successfully!", filepath);
            }


            /**
             * Returns the ID associated with the given asset name
             *
             * @param  name Asset filename (extension included)
             * @return      Corresponding asset ID, or \c 0 if not found
             */
            static uint32_t GetAssetIDByName(const string& name)
            {
                auto it = m_NameToIDList.find(name);
                if (it != m_NameToIDList.end())
                {
                    return it->second;
                }
                return 0;
            }

            /**
             * Removes an asset from both \c m_assetList and \c m_NameToIDList
             * using its filename
             *
             * @param name Asset filename (extension included)
             */
            static void DeleteByName(const string& name) 
            {
                auto nameIt = m_NameToIDList.find(name);
                if (nameIt == m_NameToIDList.end())
                {
                    Debug::Logger::LogErrorWithType(ErrorType::AssetNotFound,
                        "Asset '{}' to delete not found!", name);
                    return;
                }

                uint32_t id = nameIt->second;
                m_assetList.erase(id);
                m_NameToIDList.erase(nameIt);
                m_IDToNameList.erase(id);

                GKC_ENGINE_INFO("Asset '{}' deleted successfully!", name);
            }

            /**
             * Removes an asset from both \c m_assetList and \c m_NameToIDList
             * using its ID
             *
             * @param id Asset ID
             */
            static void DeleteByID(uint32_t id)
            {
                if (!AssetExistsByID(id))
                {
                    Debug::Logger::LogErrorWithType(ErrorType::AssetNotFound,
                        "Asset with ID: '{}' to delete not found!", id);
                    return;
                }

                for (auto it = m_NameToIDList.begin(); it != m_NameToIDList.end(); ++it)
                {
                    if (it->second == id)
                    {
                        m_NameToIDList.erase(it);
                        break;
                    }
                }

                m_IDToNameList.erase(id);
                m_assetList.erase(id);
                GKC_ENGINE_INFO("Asset with ID: '{}' deleted successfully!", id);
            }


            static bool AssetExistsByName(const string& name)
            {
                return m_NameToIDList.contains(name);
            }

            static bool AssetExistsByID(uint32_t id)
            {
                return m_assetList.contains(id);
            }

            /**
             * Retrieves a raw pointer to the underlying asset by its filename,
             * returns \c nullptr if the asset doesn't exist or hasn't been loaded yet
             *
             * @param  name Asset filename (extension included)
             * @return      Raw pointer to the \c AssetType, or \c nullptr on failure
             */
            static AssetType* GetAssetByName(const string& name)
            {
                auto nameIt = m_NameToIDList.find(name);
                if (nameIt == m_NameToIDList.end())
                {
                    Debug::Logger::LogErrorWithType(ErrorType::AssetNotFound,
                        "'{}' doesn't exist!", name);
                    return nullptr;
                }

                return GetAssetByID(nameIt->second);
            }

            /**
             * Retrieves a raw pointer to the underlying asset by its ID,
             * returns \c nullptr if the asset doesn't exist or hasn't been loaded yet
             *
             * @param  id Asset ID
             * @return    Raw pointer to the \c AssetType, or \c nullptr on failure
             */
            static AssetType* GetAssetByID(uint32_t id)
            {   
                auto it = m_assetList.find(id);
                if (it == m_assetList.end())
                {
                    Debug::Logger::LogErrorWithType(ErrorType::AssetNotFound,
                        "{} with ID '{}' to retrieve doesn't exist!",
                        Debug::GetCleanTypename<AssetType>(), id);
                    return nullptr;
                }

                if (it->second == nullptr)
                {
                    Debug::Logger::LogErrorWithType(ErrorType::AssetIsNull,
                        "{} with ID '{}' is NULL!",
                        Debug::GetCleanTypename<AssetType>(), id);
                    return nullptr;
                }

                AssetType* asset = it->second->GetRawAsset();
                if (asset == nullptr)
                {
                    Debug::Logger::LogErrorWithType(ErrorType::AssetNotLoaded,
                        "{} with ID '{}' exists but hasn't been loaded yet!",
                        Debug::GetCleanTypename<AssetType>(), id);
                }

                return asset;
            }

            /**
             * Retrieves a raw pointer to the \c AssetInfo wrapper of an asset
             * by its filename, returns \c nullptr if the info doesn't exist or is NULL
             *
             * @param  name Asset filename (extension included)
             * @return      Raw pointer to the \c AssetInfo, or \c nullptr on failure
             */
            static AssetInfo* GetAssetInfo(const string& name)
            {
                auto nameIt = m_NameToIDList.find(name);
                if (nameIt == m_NameToIDList.end())
                {
                    Debug::Logger::LogErrorWithType(ErrorType::AssetNotFound,
                        "'{}' information doesn't exist!", name);
                    return nullptr;
                }

                auto it = m_assetList.find(nameIt->second);
                if (it == m_assetList.end() || it->second == nullptr)
                {
                    Debug::Logger::LogErrorWithType(ErrorType::AssetIsNull,
                        "'{}' information is NULL!", name);
                    return nullptr;
                }

                return it->second.get();
            }

            /**
             * Helper that returns the memory address of the underlying asset
             * held by an \c AssetInfo instance as a \c const \c void*
             *
             * @param  assetInfo Pointer to an \c AssetInfo instance
             * @return           Address of the asset, or \c nullptr if unloaded or NULL
             */
            static const void* GetAddressOfAsset(const AssetInfo* assetInfo)
            {
                if (assetInfo != nullptr)
                {
                    const AssetType* asset = assetInfo->GetRawAsset();
                    if (asset != nullptr)
                    {
                        return CastToVoidPtr(asset);
                    }
                    return nullptr;
                }
                return nullptr;
            }

            /**
             * Logs all entries in \c m_assetList, printing each asset's ID
             * and memory address. Unloaded assets are reported as \c nullptr
             */
            static void PrintList()
            {
                GKC_ENGINE_INFO("##### {} List #####", Debug::GetCleanTypename<AssetType>());
                for (auto& [id, info] : m_assetList)
                {
                    const void* assetAddress = GetAddressOfAsset(info.get());

                    if (assetAddress == nullptr)
                    {
                        GKC_ENGINE_INFO("ID: {0} | Address: {1}", id, "nullptr (needs to be loaded)");
                    }
                    else
                    {
                        GKC_ENGINE_INFO("ID: {0} | Address: {1}", id, assetAddress);
                    }
                }
                GKC_ENGINE_INFO("#########################");
            }

            /**
             * Iterates over all entries in \c m_assetList and loads each one
             * via \c LoadAsset, any additional arguments required by the asset's
             * \c LoadAsset method can be forwarded. \c PrintList is called
             * automatically once all assets have been processed
             *
             * @param args Additional arguments forwarded to each \c LoadAsset call
             *
             * @note Assets must be registered beforehand via \c RegisterAsset,
             * this method does not register new entries
             */
            template<typename... Args>
            static void LoadAllAssets(Args&&... args)
            {
                if (m_assetList.empty())
                {
                    GKC_ENGINE_INFO("No {}/s to load!", Debug::GetCleanTypename<AssetType>());
                    return;
                }

                for (auto& [id, info] : m_assetList)
                {
                    if (info == nullptr)
                    {
                        Debug::Logger::LogErrorWithType(ErrorType::AssetIsNull,
                            "Asset info with ID '{}' is NULL, skipping!", id);
                        continue;
                    }

                    GKC_ENGINE_DEBUG("Loading {} with ID: '{}'",
                        Debug::GetCleanTypename<AssetInfo>(), id);
                    LoadAsset(info->GetFilepath(), std::forward<Args>(args)...);
                }

                PrintList();
            }

            static TypeList& GetAssetList() { return m_assetList; }
            static const unordered_map<string, uint32_t>& GetNameToIDList() { return m_NameToIDList; }
            static const unordered_map<uint32_t, string>& GetIDToNameList() { return m_IDToNameList; } 
    };
}   