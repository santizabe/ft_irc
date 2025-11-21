#ifndef DEBUG_HPP
# define DEBUG_HPP

#include "headers.hpp"

// Global debug flag
extern bool g_debugMode;

// Debug macros
#define DEBUG_LOG(msg) if (g_debugMode) { std::cout << "[DEBUG] " << msg << std::endl; }
#define DEBUG_LOG_CLIENT(client, msg) if (g_debugMode) { std::cout << "[DEBUG][Client:" << client->getNickname() << "] " << msg << std::endl; }
#define DEBUG_LOG_CHANNEL(channel, msg) if (g_debugMode) { std::cout << "[DEBUG][Channel:" << channel->getName() << "] " << msg << std::endl; }
#define DEBUG_LOG_SERVER(msg) if (g_debugMode) { std::cout << "[DEBUG][Server] " << msg << std::endl; }
#define DEBUG_LOG_CMD(cmd, msg) if (g_debugMode) { std::cout << "[DEBUG][" << cmd << "] " << msg << std::endl; }

// Debug functions
void enableDebugMode();
void disableDebugMode();
bool isDebugMode();

#endif
