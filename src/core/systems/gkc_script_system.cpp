#include <core/systems/gkc_script_system.h>
#include <core/managers/gkc_script_man.h>
#include <core/managers/gkc_ecs_man.h>
#include <ecs/gkc_components.h>
#include <core/events/gkc_dispatcher.h>
#include <core/gkc_logger.h>

using namespace Galaktic::Core;
using namespace Galaktic::Core::Managers;

Systems::ScriptSystem::ScriptSystem(Managers::ECS_Manager& manager)
    : m_ecsManager(manager) {}

void Systems::ScriptSystem::OnEvent(Events::GKC_Event& e) {
    Events::GKC_EventDispatcher dispatcher(e);

    dispatcher.DispatchEvent<Events::ScriptCalled>(
        [this](Events::ScriptCalled& e) {
            auto script = ScriptManager::GetScriptFromID(e.GetScriptID());
            GKC_CHECK_RUN_SCRIPT(script, e.GetScriptID());
            return false;
        });

    dispatcher.DispatchEvent<Events::ScriptCalledFromEntity>(
        [this](Events::ScriptCalledFromEntity& e) {
            auto entity = m_ecsManager.GetEntityByID(e.GetEntityID());
            if(entity != nullptr) {     
                ECS::IfComponentExists<ECS::ScriptComponent>(*entity, [&]() {
                    auto& scriptComp = entity->Get<ECS::ScriptComponent>();
                    auto script = ScriptManager::GetScriptFromID(scriptComp.id_);
                    GKC_CHECK_RUN_SCRIPT(script, scriptComp.id_);
                });
            } 
            return false;
        });
}