#pragma once

#include <Arduino.h>
#include <Preferences.h>
#include "../driver/IStepperDriver.h"
#include "../driver/DriverFactory.h"
#include "CommandParser.h"

// =============================================================================
// CLI — Command line interface dispatcher with NVS persistence.
// =============================================================================

class CLI {
public:
    explicit CLI(IStepperDriver* driver = nullptr);
    ~CLI();

    void begin();
    void loop();

private:
    IStepperDriver* _driver;
    Preferences     _prefs;
    char            _buf[128];
    uint8_t         _bufLen = 0;

    void _processLine(char* line);
    void _printPrompt();

    // NVS persistence
    void _loadSettings();
    void _saveDriver(const char* name);
    void _saveConfig();

    // Command handlers
    void _cmdTestStep(const ParsedCommand& cmd);
    void _cmdTestStop(const ParsedCommand& cmd);
    void _cmdDriverSelect(const ParsedCommand& cmd);
    void _cmdDriverStatus(const ParsedCommand& cmd);
    void _cmdDriverConfig(const ParsedCommand& cmd);
    void _cmdHelp(const ParsedCommand& cmd);
    void _unknownCommand(const ParsedCommand& cmd);

    bool _requireDriver();
};
