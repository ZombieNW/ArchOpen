#pragma once

#include <string>

namespace logger {
    void logInfo(const std::string& message);
    void logWarning(const std::string& message);
    void logError(const std::string& message);
    void logDebug(const std::string& message);
    void logSuccess(const std::string& message);
}