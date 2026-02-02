#include <filesys/gkc_reader.h>
#include "core/managers/gkc_ecs_man.h"
#include <ecs/gkc_registry.h>

#include "core/gkc_scene.h"

using namespace Galaktic;

void Filesystem::FileReader::ReadString(ifstream &file, string &str) {
    GKC_ENSURE_FILE_OPEN(file, Debug::ReadingException);
    using namespace ECS;

    size_t len = 0;
    file.read(GKC_READ_BINARY(len), sizeof(len));
    if (len >= 1024)
        return;
    str.resize(len);
    file.read(str.data(), len);
}

// @todo Add a function to convert old save files into new formats

void Filesystem::FileReader::ReadEntity(ifstream &file, Core::Managers::ECS_Manager& manager,
    ECS::Registry* registry) {
    GKC_ENSURE_FILE_OPEN(file, Debug::ReadingException);
    using namespace ECS;

    size_t entitySize = 0;
    unsigned int entityVersion = 0;
    EntityID id = 0;

    file.read(GKC_READ_BINARY(entitySize), sizeof(entitySize));
    Read(file, entityVersion);
    Read(file, id);

    Entity entity(id, registry); entity.SetID(id);
    manager.AddEmptyEntity(id, entity);
    registry->ForEachRegisteredComponent([&](const ComponentTypeInfo& info) {
        if (info.isTag_) {
            manager.AddTagByType(id, info.type_);
            return;
        }

        any component;
        info.deserialize(component, file);
        manager.AddRawComponentToEntity(id, info.type_, std::move(component));
    });

    #if GKC_DEBUG
        GKC_ENGINE_INFO("Read entity (ID: {0}): successfully", id);
    #endif
}

void Filesystem::FileReader::ReadScene(const path& path, Core::Managers::ECS_Manager& manager,
    ECS::Registry* registry, Core::Scene& scene) {
    using namespace ECS;

    GKC_ENGINE_INFO("Reading scene from {}", path.string());
    ifstream file(path, std::ios::binary);
    GKC_ENSURE_FILE_OPEN(file, Debug::ReadingException);

    size_t sceneSize = 0;
    unsigned int version = 0;
    string sceneName;

    file.read(GKC_READ_BINARY(sceneSize), sizeof(sceneSize));
    Read(file, version);
    if (version != GKC_VERSION_SCENE) {
        if (version < GKC_VERSION_SCENE) {
            GKC_ENGINE_ERROR("This scene cannot be read by this function, use a newer version");
            return;
        }
        if (version > GKC_VERSION_SCENE) {
            GKC_ENGINE_ERROR("This scene cannot be read by this function, use an older version");
            return;
        }
    }

    ReadString(file, sceneName);
    scene.m_sceneInfo.scene_name_ = sceneName;

    // Read entities until footer string
    while (true) {
        std::streampos entityPos = file.tellg();
        if (!file.good())
            break;

        size_t stringSize = 0;
        file.read(GKC_READ_BINARY(stringSize), sizeof(stringSize));
        if (!file.good()) {
            break;
        }

        // Avoid wrong readings
        if (stringSize > 1024) {
            file.seekg(entityPos);
            ReadEntity(file, manager, registry);
            continue;
        }

        string footer(stringSize, '\0');
        file.read(footer.data(), stringSize);

        if (!file.good()) {
            break;
        }

        // End when the footer is found
        if (footer == "Wrote in Galaktic ^o^") {
            break;
        }

        file.seekg(entityPos);
        ReadEntity(file, manager, registry);
    }

    file.close();
    GKC_ENGINE_INFO("'{}' scene was read successfully!", scene.m_sceneInfo.scene_name_);
}