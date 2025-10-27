#pragma once

#include <string>

namespace styles {
    inline std::string bold(const std::string& text) {
        return "\033[1m" + text + "\033[0m";
    }

    inline std::string underline(const std::string& text) {
        return "\033[4m" + text + "\033[0m";
    }

    inline std::string red(const std::string& text) {
        return "\033[31m" + text + "\033[0m";
    }

    inline std::string green(const std::string& text) {
        return "\033[32m" + text + "\033[0m";
    }

    inline std::string yellow(const std::string& text) {
        return "\033[33m" + text + "\033[0m";
    }

    inline std::string blue(const std::string& text) {
        return "\033[34m" + text + "\033[0m";
    }

    inline std::string magenta(const std::string& text) {
        return "\033[35m" + text + "\033[0m";
    }
}