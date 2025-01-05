#include <logger.h>
#include <iostream>

Logger& Logger::_get_singleton()
{
    static Logger _obj;
    return _obj;
}

Logger::Logger()
{
    m_redir = [](const Logger::type& t, const std::string& s) {
        switch(t) {
        case type::T_INFO:  std::cout << "[INFO] "  << s << std::endl; break;
        case type::T_WARN:  std::cout << "[WARN] "  << s << std::endl; break;
        case type::T_ERROR: std::cout << "[ERROR] " << s << std::endl; break;
        case type::T_DEBUG: std::cout << "[DEBUG] " << s << std::endl; break;
        }
    };
}


void Logger::set_depth(const Logger::type& n)
{
    Logger::_get_singleton().m_depth = n;
}

void Logger::set_callback(std::function<void(const Logger::type&, const std::string&)> f)
{
    Logger::_get_singleton().m_redir = f;
}

void Logger::print(const Logger::type& t, const std::string& s)
{
    auto& thus = Logger::_get_singleton();
    
    if (thus.m_redir && t <= thus.m_depth) {
        std::lock_guard<std::mutex> l(thus.m_mtx);
        thus.m_redir(t, s);
    }
}