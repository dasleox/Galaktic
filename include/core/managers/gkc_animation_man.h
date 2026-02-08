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
    class Animation;
    struct AnimationInfo;
    typedef unordered_map<string, unique_ptr<Render::AnimationInfo>> Animation_List;
    typedef unordered_map<AnimationID, string> AnimationID_List;
}

namespace Galaktic::Core::Managers {
    class AnimationManager {
        public:
            explicit AnimationManager(const path& folderPath);
            static void AddAnimationPath(const path& filePath);
            static void AddAnimation(const path& filePath, SDL_Renderer* renderer);
            
            static void LoadAnimation(const path& filePath, SDL_Renderer* renderer);
            static void LoadAllAnimations(SDL_Renderer* renderer);
            
            static void DeleteAnimation(const string& name);
            
            static Render::Animation* GetAnimation(const string& name);
            static Render::Animation* GetAnimation(AnimationID id);
            
            static Render::AnimationInfo* GetAnimationInfo(const string& name);
            
            static void UpdateAll(float deltaTime);
            static void PrintList();
            
        private:
            static Render::Animation_List m_animationList;
            static Render::AnimationID_List m_IDToNameList;
            static vector<path> m_animationPathList;
            static const void* TakeAddressOfAnimation(const Render::AnimationInfo* animInfo);
    };
}