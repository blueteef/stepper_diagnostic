// =============================================================================
// main.cpp
// =============================================================================

#include <Arduino.h>
#include "config.h"
#include "cli/CLI.h"
#include "driver/a4988/A4988Driver.h"

// Default driver for first boot. Once a driver is selected via CLI,
// NVS persistence takes over and this fallback is ignored.
static CLI shell(new A4988Driver());

void setup() {
    Serial.begin(CLI_BAUD);
    delay(500);

    // Output pins
    pinMode(PIN_STEP,   OUTPUT); digitalWrite(PIN_STEP,   LOW);
    pinMode(PIN_DIR,    OUTPUT); digitalWrite(PIN_DIR,    LOW);
    pinMode(PIN_ENABLE, OUTPUT); digitalWrite(PIN_ENABLE, HIGH);  // disabled
    pinMode(PIN_MS1,    OUTPUT); digitalWrite(PIN_MS1,    LOW);
    pinMode(PIN_MS2,    OUTPUT); digitalWrite(PIN_MS2,    LOW);
    pinMode(PIN_MS3,    OUTPUT); digitalWrite(PIN_MS3,    LOW);

    // PIN_SPREAD = socket SLEEP — HIGH keeps A4988 awake
    pinMode(PIN_SPREAD, OUTPUT); digitalWrite(PIN_SPREAD, HIGH);

    // PIN_UART = socket RESET — HIGH keeps A4988 out of reset
    pinMode(PIN_UART,   OUTPUT); digitalWrite(PIN_UART,   HIGH);

    // Input pins
    pinMode(PIN_DIAG,  INPUT_PULLUP);
    pinMode(PIN_INDEX, INPUT_PULLUP);

    shell.begin();
}

void loop() {
    shell.loop();
}
