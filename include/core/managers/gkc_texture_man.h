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

namespace Galaktic::Render {
    struct TextureInfo;
    class Texture;
    typedef unordered_map<string, shared_ptr<TextureInfo>> Texture_List;
    typedef unordered_map<TextureID, string> TextureID_List;
}

namespace Galaktic::ECS { 
    class Entity;
    typedef unordered_map<EntityID, Entity> Entity_List;
}

namespace Galaktic::Core::Managers {
    /**
    * @class TextureManager
    * Manages all textures inside a specified folder, all files that are not situable
    * for textures are automatically ignored. The textures are put inside
    * the texture list an can be accessed statically from anywhere in the engine.
    * However, a previous initialization of this manager is required.
    * 
    * When this manager is initialized textures need to be loaded afterwards
    * using \c LoadAllTextures() , individual textures can also be loaded, if they
    * don't exist they will be automatically added to the texture list
    * 
    * Textures are stored in the list with their filename (extension included: e.g. texture.png)
    * as a key and the value stored inside a TextureInfo struct.
    */
    class TextureManager {
        public:
            /**
             * @param path folder path
             */
            TextureManager(const string& path);

            /**
             * @brief Adds a texture to this instance's list
             * @param path path to the texture (relative or full)
             * @param renderer SDL_Renderer
             */
            static void AddTexture(const string &path, SDL_Renderer *renderer);

            /**
             * @brief Adds a texture but only the path is added to the list
             * @param path path to the texture (relative or full)
             */
            static void AddTexturePath(const string& path);

            /**
             * Loads a texture with a SDL_Renderer, texture key has to already
             * exist in the list with \c AddTexturePath() , if the texture key wasn't
             * in the list it will be automatically added.
             * @param path Texture's path
             * @param renderer SDL_Renderer
             */
            static void LoadTexture(const string& path, SDL_Renderer* renderer);

            /**
             * @brief Loads all textures that need to be loaded, all texture's paths
             * were automatically added inside \c m_texturePathList
             */
            static void LoadAllTextures(SDL_Renderer* renderer);
            
            /**
             * @brief Deletes the texture on the list
             * @param name The name of the texture
             * @note The input SHOULD be the key of the list, not the path as seen here,
             *       this is a string not a path ;)
             */
            static void DeleteTexture(const string &name);

            /**
             * @brief Gets a pointer to the texture in the list
             * @param name The name of the texture
             * @return Pointer to the Texture
             * @note The input SHOULD be the key of the list, not the path as seen here
             *       this is a string not a path ;)
             */
            static shared_ptr<Render::Texture> GetTextureByName(const string &name);

            /**
             * Gets a pointer to the texture in the list, if the texture doesn't
             * exist in the list it returns nullptr
             * @param id The ID of the texture
             * @return Pointer to the Texture if it exists, nullptr otherwise
             */
            static shared_ptr<Render::Texture> GetTextureByID(TextureID id);

            /**
             * Gets a pointer to the texture info, this function is used to retrieve
             * the info (ID + Texture's instance), this function is used by the
             * texture helper which automatically assigns textures to entities
             * that have a texture component.
             * 
             * @see gkc_texture_helper.h for more information
             * @param textureName The name of the texture
             * @return Render::TextureInfo* 
             */
            static shared_ptr<Render::TextureInfo> GetTextureInfo(const string& textureName);


            static SDL_Texture* GetMissingTexture();

            static void CreateMissingTexture(SDL_Renderer* renderer);

            /**
             * @brief Destroys the missing texture to free memory
             */
            static void DestroyMissingTexture();

            /**
             * Prints the list of all texture files with their filenames,
             * ID's and address in memory
             */
            static void PrintList();

            static Render::Texture_List& GetTextureList() { return m_textureList; }
            static Render::TextureID_List& GetIDTextureList() { return m_IDToNameList; } 
            static vector<path>& GetTexturePathList() { return m_texturePathList; }
        private:
            static Render::Texture_List m_textureList;
            static vector<path> m_texturePathList;
            static Render::TextureID_List m_IDToNameList;
            static SDL_Texture* m_missingTexture;
        private:
            static const void* TakeAdressOfTexture(const Render::TextureInfo* textureInfo);
    };
}