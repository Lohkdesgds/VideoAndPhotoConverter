#pragma once

#include <string>
#include <functional>
#include <mutex>

class Logger {
public:
    enum class type { INFO, WARN, ERROR, DEBUG };
private:
    type m_depth = type::ERROR;
    std::function<void(const type&, const std::string&)> m_redir;
    std::mutex m_mtx;

    static Logger& _get_singleton();
public:
    Logger();
    
    static void set_depth(const type&);
    static void set_callback(std::function<void(const type&, const std::string&)>);

    static void print(const type&, const std::string&);
};