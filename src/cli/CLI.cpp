#include "CLI.h"
#include <stdlib.h>

CLI::CLI(IStepperDriver* driver) : _driver(driver), _bufLen(0) {
    _buf[0] = '\0';
}

CLI::~CLI() {
    delete _driver;
}

void CLI::begin() {
    Serial.println(F("\nStepper Driver Diag"));
    if (_driver) {
        _driver->begin();
        Serial.print(F("Driver : ")); Serial.println(_driver->driverName());
    } else {
        Serial.println(F("No driver loaded. Use: driver select <name>"));
    }
    Serial.println(F("Type 'help' for commands."));
    _printPrompt();
}

// -----------------------------------------------------------------------------
// Line editor
// -----------------------------------------------------------------------------

void CLI::loop() {
    while (Serial.available()) {
        char c = Serial.read();

        if (c == '\r') continue;

        if (c == '\n') {
            _buf[_bufLen] = '\0';
            if (_bufLen > 0) {
                Serial.println();
                _processLine(_buf);
            }
            _bufLen = 0;
            _printPrompt();
            return;
        }

        if (c == '\b' || c == 127) {
            if (_bufLen > 0) { _bufLen--; Serial.print(F("\b \b")); }
            return;
        }

        if (_bufLen < sizeof(_buf) - 1) {
            Serial.print(c);
            _buf[_bufLen++] = c;
        }
    }
}

// -----------------------------------------------------------------------------
// Dispatch
// -----------------------------------------------------------------------------

void CLI::_processLine(char* line) {
    ParsedCommand cmd;
    if (!CommandParser::parse(line, cmd)) return;

    if (cmd.verb == "test") {
        if      (cmd.noun == "step") _cmdTestStep(cmd);
        else if (cmd.noun == "stop") _cmdTestStop(cmd);
        else _unknownCommand(cmd);
    }
    else if (cmd.verb == "driver") {
        if      (cmd.noun == "select") _cmdDriverSelect(cmd);
        else if (cmd.noun == "status") _cmdDriverStatus(cmd);
        else if (cmd.noun == "config") _cmdDriverConfig(cmd);
        else _unknownCommand(cmd);
    }
    else if (cmd.verb == "help") { _cmdHelp(cmd); }
    else _unknownCommand(cmd);
}

// -----------------------------------------------------------------------------
// Helpers
// -----------------------------------------------------------------------------

void CLI::_printPrompt() { Serial.print(F("> ")); }

bool CLI::_requireDriver() {
    if (_driver) return true;
    Serial.println(F("Error: no driver loaded. Use: driver select <a4988|tmc2209>"));
    return false;
}

// -----------------------------------------------------------------------------
// Command handlers
// -----------------------------------------------------------------------------

void CLI::_cmdTestStep(const ParsedCommand& cmd) {
    if (!_requireDriver()) return;

    uint32_t steps      = (uint32_t)atol(cmd.get("steps", "200").c_str());
    uint32_t speed      = (uint32_t)atol(cmd.get("speed", "200").c_str());
    uint16_t usteps     = (uint16_t)atol(cmd.get("usteps", "0").c_str());
    std::string dir     = cmd.get("dir", "");

    if (steps == 0)  { Serial.println(F("Error: --steps must be > 0")); return; }
    if (speed == 0)  { Serial.println(F("Error: --speed must be > 0")); return; }

    if (!dir.empty()) {
        if      (dir == "fwd") _driver->setDirection(true);
        else if (dir == "rev") _driver->setDirection(false);
        else { Serial.println(F("Error: --dir must be fwd or rev")); return; }
    }

    if (usteps > 0) _driver->setMicrosteps(usteps);

    _driver->enable(true);

    Serial.print(F("Stepping ")); Serial.print(steps);
    Serial.print(F(" steps @ ")); Serial.print(speed);
    Serial.println(F(" steps/sec..."));

    _driver->step(steps, speed);

    Serial.println(F("Done."));
    if (_driver->isFaultActive()) Serial.println(F("WARNING: FAULT pin asserted."));
}

void CLI::_cmdTestStop(const ParsedCommand& cmd) {
    if (!_requireDriver()) return;
    _driver->stop();
    Serial.println(F("Stopped."));
}

void CLI::_cmdDriverSelect(const ParsedCommand& cmd) {
    if (!cmd.hasTarget()) {
        Serial.println(F("Usage: driver select <a4988|tmc2209>"));
        return;
    }

    IStepperDriver* next = DriverFactory::create(cmd.target.c_str());
    if (!next) {
        Serial.print(F("Unknown driver: ")); Serial.println(cmd.target.c_str());
        Serial.print(F("Supported: "));
        for (const char* const* p = DriverFactory::supportedDrivers(); *p; p++) {
            Serial.print(*p); Serial.print(' ');
        }
        Serial.println();
        return;
    }

    delete _driver;
    _driver = next;
    _driver->begin();

    Serial.print(F("Driver selected: ")); Serial.println(_driver->driverName());
}

void CLI::_cmdDriverStatus(const ParsedCommand& cmd) {
    if (!_requireDriver()) return;
    Serial.println(_driver->getStatusReport());
}

void CLI::_cmdDriverConfig(const ParsedCommand& cmd) {
    if (!_requireDriver()) return;

    bool changed = false;

    std::string ustepsStr = cmd.get("usteps", "");
    if (!ustepsStr.empty()) {
        uint16_t u = (uint16_t)atol(ustepsStr.c_str());
        _driver->setMicrosteps(u);
        Serial.print(F("Microsteps set to 1/")); Serial.println(u);
        changed = true;
    }

    std::string dirStr = cmd.get("dir", "");
    if (!dirStr.empty()) {
        if      (dirStr == "fwd") { _driver->setDirection(true);  Serial.println(F("Direction: forward")); changed = true; }
        else if (dirStr == "rev") { _driver->setDirection(false); Serial.println(F("Direction: reverse")); changed = true; }
        else Serial.println(F("Error: --dir must be fwd or rev"));
    }

    if (cmd.flag("spread")) {
        _driver->setSpreadCycle(true);
        Serial.println(F("Mode: SpreadCycle"));
        changed = true;
    } else if (cmd.flag("stealthchop")) {
        _driver->setSpreadCycle(false);
        Serial.println(F("Mode: StealthChop"));
        changed = true;
    }

    std::string currentStr = cmd.get("rms", "");
    if (!currentStr.empty()) {
        uint16_t mA = (uint16_t)atol(currentStr.c_str());
        _driver->setCurrentMilliamps(mA);
        Serial.print(F("Current: ")); Serial.print(mA); Serial.println(F(" mA RMS"));
        changed = true;
    }

    if (!changed) Serial.println(F("Usage: driver config [--usteps 1|2|4|8|16] [--dir fwd|rev] [--rms N] [--spread|--stealthchop]"));
}

void CLI::_cmdHelp(const ParsedCommand& cmd) {
    Serial.println(F(
        "\nCommands:\n"
        "  test step [--steps N] [--speed N] [--dir fwd|rev] [--usteps 1|2|4|8|16]\n"
        "  test stop\n"
        "  driver select <a4988|tmc2208|tmc2209>\n"
        "  driver status\n"
        "  driver config [--usteps 1|2|4|8|16] [--dir fwd|rev]\n"
        "                [--rms N] [--spread] [--stealthchop]\n"
        "  help\n"
        "\nDefaults: steps=200 speed=200 (steps/sec)\n"
        "Note: --rms, --spread, --stealthchop are TMC2208/TMC2209-only.\n"
    ));
}

void CLI::_unknownCommand(const ParsedCommand& cmd) {
    Serial.print(F("Unknown command: "));
    Serial.print(cmd.verb.c_str());
    if (cmd.hasNoun()) { Serial.print(' '); Serial.print(cmd.noun.c_str()); }
    Serial.println(F("\nType 'help' for commands."));
}
