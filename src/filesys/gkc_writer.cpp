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

#include <filesys/gkc_writer.h>
#include "core/gkc_error.h"
#include "core/gkc_logger.h"
#include "ecs/gkc_components.h"
#include "ecs/gkc_entity.h"
#include <ecs/gkc_registry.h>
#include "ecs/gkc_component_pool.h"

#include "core/gkc_logger.h"
#include "core/gkc_scene.h"
#include "core/managers/gkc_ecs_man.h"
#include "ecs/gkc_component_registry.h"
#include <filesys/gkc_filesys.h>
using namespace Galaktic;

void Filesystem::FileWriter::WriteString(ofstream &file, const string &str) {
    GKC_ENSURE_FILE_OPEN(file, WritingError);
    auto len = static_cast<Uint32>(str.size());
    file.write(GKC_WRITE_BINARY(len), sizeof(len));
    file.write(str.data(), len);
}

void Filesystem::FileWriter::WriteEntity(ofstream &file, const ECS::Entity &entity,
                                         ECS::Registry* registry) {
    using namespace ECS;
    GKC_ENSURE_FILE_OPEN(file, WritingError);

    size_t entitySize = 0;
    EntityID id = entity.GetID();

    entitySize += registry->GetAllComponentsSize(id);

    entitySize += sizeof(unsigned int) + sizeof(unsigned int);

    file.write(GKC_WRITE_BINARY(entitySize), sizeof(entitySize));
    Write(file, GKC_VERSION_ENTITY);
    Write(file, entity.GetID());

    auto& pools = registry->GetComponentPools();
    
    for (auto& [typeIndex, pool] : pools) {
        if (!pool->Contains(id))
            continue;
            
        const ComponentTypeInfo& info = ComponentRegistry::Get(typeIndex);
        
        if (info.isTag)
            continue;

        pool->Serialize(id, file);
    }

    #if GKC_DEBUG
        GKC_ENGINE_INFO("Writing {0} bytes for entity {1}", entitySize, id);
    #endif
}

void Filesystem::FileWriter::WriteScene(const path& path, Core::Scene& scene, ECS::Registry* registry) {
    using namespace ECS;
    GKC_ENGINE_INFO("Writing scene {0} in {1}", scene.m_sceneInfo.scene_name_, path.string());
    ofstream file(path.filename(), std::ios::binary);
    GKC_ENSURE_FILE_OPEN(file, WritingError);

    size_t sceneSize = scene.m_sceneInfo.scene_name_.length() + sizeof(GKC_VERSION_SCENE);

    for (auto& pair : scene.GetECSManager()->GetEntityList()) {
        sceneSize += scene.GetRegistry()->GetAllComponentsSize(pair.first);
    }

    file.write(GKC_WRITE_BINARY(sceneSize), sizeof(sceneSize));
    Write(file, GKC_VERSION_SCENE);
    WriteString(file, scene.m_sceneInfo.scene_name_);

    for (auto& pair : scene.GetECSManager()->GetEntityList()) {
        #if GKC_DEBUG
            GKC_ENGINE_INFO("Entity #{}: &{}", pair.first, Core::CastToVoidPtr(pair.second));
        #endif
        WriteEntity(file, pair.second, registry);
    }

    WriteString(file, "Wrote in Galaktic ^o^");
    file.flush();
    file.close();


    auto filepath = path.filename().string();
    auto desiredPath = (path.parent_path() / GKC_SCENE_PATH / path.filename()).string();
    if (CheckFile(desiredPath)) {
        RemoveFile(desiredPath);
    }

    std::rename(filepath.c_str(), desiredPath.c_str());
    GKC_ENGINE_INFO("'{}' scene was written successfully!", scene.m_sceneInfo.scene_name_);
}
