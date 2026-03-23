#pragma once

#include <Arduino.h>
#include "../driver/IStepperDriver.h"
#include "../driver/DriverFactory.h"
#include "CommandParser.h"

// =============================================================================
// CLI — Command line interface dispatcher.
// =============================================================================

class CLI {
public:
    explicit CLI(IStepperDriver* driver = nullptr);
    ~CLI();

    void begin();
    void loop();

private:
    IStepperDriver* _driver;
    char            _buf[128];
    uint8_t         _bufLen = 0;

    void _processLine(char* line);
    void _printPrompt();

    // Command handlers
    void _cmdTestStep(const ParsedCommand& cmd);
    void _cmdTestStop(const ParsedCommand& cmd);
    void _cmdDriverSelect(const ParsedCommand& cmd);
    void _cmdDriverStatus(const ParsedCommand& cmd);
    void _cmdDriverConfig(const ParsedCommand& cmd);
    void _cmdHelp(const ParsedCommand& cmd);
    void _unknownCommand(const ParsedCommand& cmd);

    bool _requireDriver();  // prints error and returns false if no driver loaded
};
