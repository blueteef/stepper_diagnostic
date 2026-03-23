#include "A4988Driver.h"
#include "../../config.h"
#include <Arduino.h>

// A4988 microstep truth table: index = log2(usteps)
// usteps:  1     2     4     8     16
// MS1:     0     1     0     1     1
// MS2:     0     0     1     1     1
// MS3:     0     0     0     0     1
static const uint8_t MS_TABLE[5][3] = {
    {0, 0, 0},  // full step
    {1, 0, 0},  // half
    {0, 1, 0},  // quarter
    {1, 1, 0},  // eighth
    {1, 1, 1},  // sixteenth
};

// Map usteps value to table index. Returns -1 if invalid.
static int ustepsToIndex(uint16_t usteps) {
    switch (usteps) {
        case  1: return 0;
        case  2: return 1;
        case  4: return 2;
        case  8: return 3;
        case 16: return 4;
        default: return -1;
    }
}

// -----------------------------------------------------------------------------

A4988Driver::A4988Driver() {}

void A4988Driver::begin() {
    // Outputs already configured in main.cpp setup(); just set safe defaults.
    digitalWrite(PIN_ENABLE, HIGH);   // disabled
    digitalWrite(PIN_DIR,    LOW);
    digitalWrite(PIN_STEP,   LOW);

    // SLEEP and RESET (PIN_SPREAD, PIN_UART) are driven HIGH in main.cpp setup.
    // Nothing extra needed here.

    _applyMicrostepPins();
}

void A4988Driver::enable(bool on) {
    _enabled = on;
    digitalWrite(PIN_ENABLE, on ? LOW : HIGH);  // active-low
}

void A4988Driver::setDirection(bool forward) {
    _forward = forward;
    digitalWrite(PIN_DIR, forward ? HIGH : LOW);
}

void A4988Driver::setMicrosteps(uint16_t usteps) {
    int idx = ustepsToIndex(usteps);
    if (idx < 0) return;  // ignore invalid value
    _usteps = usteps;
    _applyMicrostepPins();
}

void A4988Driver::_applyMicrostepPins() {
    int idx = ustepsToIndex(_usteps);
    if (idx < 0) idx = 0;
    digitalWrite(PIN_MS1, MS_TABLE[idx][0]);
    digitalWrite(PIN_MS2, MS_TABLE[idx][1]);
    digitalWrite(PIN_MS3, MS_TABLE[idx][2]);
}

void A4988Driver::step(uint32_t steps, uint32_t stepsPerSec) {
    if (stepsPerSec == 0 || steps == 0) return;

    _running = true;

    // Half-period in microseconds
    uint32_t halfUs = 1000000UL / (2 * stepsPerSec);
    if (halfUs < STEP_PULSE_US) halfUs = STEP_PULSE_US;

    for (uint32_t i = 0; i < steps && _running; i++) {
        digitalWrite(PIN_STEP, HIGH);
        delayMicroseconds(halfUs);
        digitalWrite(PIN_STEP, LOW);
        delayMicroseconds(halfUs);
    }

    _running = false;
}

void A4988Driver::stop() {
    _running = false;
}

void A4988Driver::setCurrentMilliamps(uint16_t mA) {
    // VREF_PWM not wired — using onboard trimmer pot.
    // Store the value for status reporting only.
    _currentMa = mA;
}

uint16_t A4988Driver::getCurrentMilliamps() {
    return _currentMa;
}

bool A4988Driver::isFaultActive() {
    // FAULT (PIN_DIAG) is active-low. Not connected in current wiring;
    // internal pullup means it reads HIGH (no fault) when unconnected.
    return digitalRead(PIN_DIAG) == LOW;
}

String A4988Driver::getStatusReport() {
    String s;
    s += "driver  : a4988\n";
    s += "enabled : "; s += (_enabled ? "yes" : "no"); s += "\n";
    s += "dir     : "; s += (_forward ? "forward" : "reverse"); s += "\n";
    s += "usteps  : 1/"; s += _usteps; s += "\n";
    s += "current : ";
    if (_currentMa > 0) { s += _currentMa; s += " mA (stored)\n"; }
    else                { s += "N/A (set via onboard pot)\n"; }
    s += "fault   : "; s += (isFaultActive() ? "ACTIVE" : "none");
    return s;
}
