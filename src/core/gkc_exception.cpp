#include <core/gkc_exception.h>

using namespace Galaktic::Debug;

GalakticException::GalakticException(const string &message, const string &file,
     const string& function, int line)
        : m_message(message), m_file(file), m_function(function), m_line(line) {
    BuildWhat();
}

void GalakticException::BuildWhat() {
    m_what = "GalakticException: " + m_message +
            "\n File: " + m_file +
            "\n Function: " + m_function +
            "\n Line: " + std::to_string(m_line);
}