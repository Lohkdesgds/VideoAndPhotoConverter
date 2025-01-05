#pragma once

#include <string>
#include <functional>
#include <mutex>

class Logger {
public:
    enum class type { T_INFO, T_WARN, T_ERROR, T_DEBUG };
private:
    type m_depth = type::T_ERROR;
    std::function<void(const type&, const std::string&)> m_redir;
    std::mutex m_mtx;

    static Logger& _get_singleton();
public:
    Logger();
    
    static void set_depth(const type&);
    static void set_callback(std::function<void(const type&, const std::string&)>);

    static void print(const type&, const std::string&);
};

#if !defined(__PRETTY_FUNCTION__) && !defined(__GNUC__)
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif


#define DBGS(...) Logger::print(Logger::type::T_DEBUG, std::string(__PRETTY_FUNCTION__) + " > " + (__VA_ARGS__));