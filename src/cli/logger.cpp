#include <iostream>

#include "logger.h"
#include "styles.h"

namespace logger {
    void logInfo(const std::string& message) {
        std::cout << styles::blue("INFO: ") << message << std::endl;
    }

    void logWarning(const std::string& message){ 
        std::cout << styles::yellow("WARNING: ") << message << std::endl;
    }

    void logError(const std::string& message) {
        std::cerr << styles::red("ERROR: ") << message << std::endl;
    }

    void logDebug(const std::string& message) {
        std::cout << styles::magenta("DEBUG: ") << message << std::endl;
    }

    void logSuccess(const std::string& message) { 
        std::cout << styles::green("SUCCESS: ") << message << std::endl;
    }
}
