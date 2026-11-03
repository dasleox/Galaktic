#include <core/gkc_logger.h>
#include <core/gkc_error.h>
#include <config/gkc_config_file.h>
#include <filesys/gkc_filesys.h>

using namespace Galaktic::Core;
using namespace Galaktic;
using namespace Galaktic::Config;

static string Trim(const string& str)
{
    const char* ws = " \t\r\n";
    auto start = str.find_first_not_of(ws);
    if(start == string::npos) 
        return {};
    auto end = str.find_last_not_of(ws);
    return str.substr(start, end - start + 1);
}

// FORMAT: variable=value;
// Comments: #comment
static string ParseName(const string& line)
{
    string trimmed = Trim(line);
    if(trimmed.empty() || trimmed[0] == '#') return {};

    size_t eq = trimmed.find('=');
    if(eq == string::npos) return {};

    return Trim(trimmed.substr(0, eq));
}


ConfigFile::ConfigFile(const path& filepath) : m_path(filepath)
{
    if(Load())
        GKC_ENGINE_INFO("Loaded configuration file: {}", filepath.filename().string());
}

bool ConfigFile::Load()
{
    ifstream file(m_path);
    GKC_ENSURE_FILE_OPEN_BOOL(file);

    m_lines.clear();
    m_variableToLineMap.clear();

    string lineStr;
    while(std::getline(file, lineStr))
    {
        m_lines.push_back(lineStr);
        string name = ParseName(lineStr);
        if(!name.empty())
            m_variableToLineMap[name] = m_lines.size() - 1;
    }
    return true;
}


bool ConfigFile::Save() const
{
    ofstream file(m_path, std::ios::trunc);
    GKC_ENSURE_FILE_OPEN_BOOL(file);

    for(const auto& line : m_lines)
        file << line << '\n';

    return true;
}

bool ConfigFile::Has(const string& varName) const
{
    return m_variableToLineMap.contains(varName);
}

bool ConfigFile::SetRaw(const string& name, const string& value)
{
    if(name.empty())
    {
        Debug::Logger::LogErrorWithType(ErrorType::EmptyString, "Variable name is empty!");
        return false;
    }

    string newLine = name + "=" + value + ";";

    auto it = m_variableToLineMap.find(name);
    if(it != m_variableToLineMap.end())
    {
        m_lines[it->second] = newLine;
    }
    else
    {
        m_lines.push_back(newLine);
        m_variableToLineMap[name] = m_lines.size() - 1;
    }
    return true;
}

std::optional<string> ConfigFile::GetRaw(const string& name) const
{
    auto it = m_variableToLineMap.find(name);
    if(it == m_variableToLineMap.end())
        return std::nullopt;

    const string& line = m_lines[it->second];
    size_t eq  = line.find('=');
    size_t end = line.find_last_of(';');
    if(end == string::npos || end < eq) end = line.size();  

    return Trim(line.substr(eq + 1, end - eq - 1));
}