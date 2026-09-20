#include <config/gkc_config_register.h>
#include <core/gkc_logger.h>
#include <core/gkc_error.h>


using namespace Galaktic::Config;
using namespace Galaktic;

void CallbackRegister::RegisterCallback(const string& callbackName, Callback callback)
{
    if(callbackName.empty())
    {
        Debug::Logger::LogErrorWithType(ErrorType::EmptyString, "Callback name is empty!");
        return;
    }

    m_callbackList.emplace(callbackName, std::move(callback));
}

const CallbackRegister::Callback* CallbackRegister::Find(const string& callbackName) const 
{
    auto it = m_callbackList.find(callbackName);
    if(it != m_callbackList.end())
    {
        return &it->second;
    }

    Debug::Logger::LogErrorWithType(ErrorType::EmptyString, "Callback name is empty!");
    return nullptr;
}