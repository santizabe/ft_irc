#include "debug.hpp"

// Initialize global debug flag - default to false
bool g_debugMode = false;

void enableDebugMode() {
    g_debugMode = true;
    std::cout << "\n[DEBUG] Debug mode enabled\n" << std::endl;
}

void disableDebugMode() {
    std::cout << "\n[DEBUG] Debug mode disabled\n" << std::endl;
    g_debugMode = false;
}

bool isDebugMode() {
    return g_debugMode;
}
