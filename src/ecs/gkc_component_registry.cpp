#include <ecs/gkc_component_registry.h>

namespace Galaktic::ECS {
    unordered_map<type_index, ComponentTypeInfo> ComponentRegistry::m_compTypes;
}
