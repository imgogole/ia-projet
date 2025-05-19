#pragma once
#include <iostream>
#include <iomanip>
#include "utilities_time.h"

namespace Debug
{
    inline std::string GetTimestamp()
    {
        float t = Time::Time();
        int total = static_cast<int>(t);
        int h = (total / 3600) % 24;
        int m = (total % 3600) / 60;
        int s = total % 60;
        std::ostringstream oss;
        oss << std::setw(2) << std::setfill('0') << h << ':'
            << std::setw(2) << std::setfill('0') << m << ':'
            << std::setw(2) << std::setfill('0') << s;
        return oss.str();
    }

    inline void Log(const std::string& message)
    {
        // Jaune pastel
        constexpr char const* YELLOW   = "\033[38;2;255;255;102m";
        constexpr char const* RESET    = "\033[0m";
        std::cout
            << "[" << YELLOW << "LOG" << RESET << "\t"
            << GetTimestamp() << "] "
            << message << std::endl;
    }

    inline void Error(const std::string& message)
    {
        // Rouge pastel
        constexpr char const* RED      = "\033[38;2;255;102;102m";
        constexpr char const* RESET    = "\033[0m";
        std::cout
            << "[" << RED    << "ERROR" << RESET << "\t"
            << GetTimestamp() << "] "
            << message << std::endl;
    }
}
