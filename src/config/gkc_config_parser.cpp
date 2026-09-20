#include <config/gkc_config_parser.h>
#include <core/gkc_error.h>
#include <core/gkc_logger.h>
#include <filesys/gkc_filesys.h>

using namespace Galaktic;
using namespace Galaktic::Debug;
using namespace Galaktic::Config;

path ConfigParser::m_configFilepath;
vector<BindEntry> ConfigParser::m_bindEntryList;

bool ConfigParser::ParseFile(const path& filepath)
{
    GKC_CHECK_PATH_INTEGRITY_BOOL(filepath);

    m_configFilepath = filepath;
    ifstream file(filepath);
    GKC_ENSURE_FILE_OPEN_BOOL(file, FileNotOpen);

    string line;
    while(std::getline(file, line))
    {   
        // Remove comments
        auto commentPos = line.find("#");
        if(commentPos != std::string::npos)
        {
            line = line.substr(0, commentPos);
        }
        line = Trim(line);

        if(line.empty()) { continue; }

        std::istringstream ss(line);
        string directive;

        ss >> directive;

        // Only accept bind keyword
        if(directive != "bind") continue;

        m_bindEntryList.push_back(ParseLine(line));
    }

    return true;
}

BindEntry ConfigParser::ParseLine(const string& line)
{
    std::istringstream ss(line);
    string directive, key, action, callbackFull;
    ss >> directive >> key >> action >> callbackFull;

    auto colonPos = callbackFull.find(':');
    if (colonPos == std::string::npos)
    {
        Logger::LogErrorWithType(ErrorType::InvalidCallback, "Malformed Callback ({}) in '{}' config file!", 
            callbackFull, m_configFilepath.string());
    }

    BindEntry entry;
    entry.bindKey      = key;
    entry.actionName   = action;
    entry.callbackType = callbackFull.substr(0, colonPos);  // C++ defined or in Lua scripts
    entry.callbackName = callbackFull.substr(colonPos + 1);  
    return entry;
}

string ConfigParser::Trim(const string& str)
{
    const char* ws = " \t\r\n";
    auto start = str.find_first_not_of(ws);
    if (start == std::string::npos) return {};
    auto end = str.find_last_not_of(ws);
    return str.substr(start, end - start + 1);
}