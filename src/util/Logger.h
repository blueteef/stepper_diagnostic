#pragma once

#include <Arduino.h>

// =============================================================================
// Logger — Lightweight timestamped serial logger.
//
// Usage:
//   LOG_INFO("Driver enabled");
//   LOG_ERR("FAULT detected on DIAG pin");
// =============================================================================

#define LOG_INFO(msg) Logger::log('I', (msg))
#define LOG_WARN(msg) Logger::log('W', (msg))
#define LOG_ERR(msg)  Logger::log('E', (msg))

class Logger {
public:
    static void log(char level, const char* msg);
    static void log(char level, const String& msg);
};
