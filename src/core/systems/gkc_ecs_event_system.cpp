#include <core/systems/gkc_ecs_event_system.h>

#include "core/events/gkc_dispatcher.h"
#include "core/managers/gkc_ecs_man.h"

using namespace Galaktic::Core;
using namespace Galaktic;

Systems::ECS_EventSystem::ECS_EventSystem(Managers::ECS_Manager &manager)
    : m_ecsManager(manager) {}

void Systems::ECS_EventSystem::OnEvent(Events::GKC_Event &event) {
    Events::GKC_EventDispatcher dispatcher(event);

    dispatcher.DispatchEvent<Events::EntityCreatedEvent> (
        [this](Events::EntityCreatedEvent &e) {
            m_ecsManager.CreateEntityByTypeIndex(e.GetEntityName(), e.GetEntityType());
            return false;
        });

    dispatcher.DispatchEvent<Events::EntityDestroyedEvent> (
        [this](Events::EntityDestroyedEvent &e) {
            m_ecsManager.DeleteEntity(e.GetEntityID());
            return false;
        });

    dispatcher.DispatchEvent<Events::EntityModifiedEvent> (
        [this](Events::EntityModifiedEvent &e) {
            m_ecsManager.AddRawComponentToEntity(e.GetEntityID(), e.GetEntityType(), e.GetEntityComponent());
           return false;
        });
}

