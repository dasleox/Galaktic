#include <ecs/gkc_component_registry.h>

using std::type_index;
using std::unordered_map;

namespace Galaktic::ECS {
    unordered_map<type_index, ComponentTypeInfo> ComponentRegistry::m_compTypes;
}
