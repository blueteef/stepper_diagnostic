#pragma once

#include <stdint.h>

// =============================================================================
// PwmVref — Sets the A4988 Vref analog voltage via PWM + RC filter.
//
// Formula:  Vref(V) = Itrip(A) * 8 * Rsense
//           Duty    = (Vref / Vcc) * (2^resolution - 1)
//
// Implemented in Phase 3.
// =============================================================================

class PwmVref {
public:
    PwmVref() = default;

    // Must be called once before setMilliamps(). Attaches the PWM channel.
    void begin();

    // Set output voltage corresponding to the given RMS current in milliamps.
    // Clamps to 0–3300 mV range.
    void setMilliamps(uint16_t mA);

    // Returns the last milliamp value set.
    uint16_t getMilliamps() const { return _mA; }

    // Returns the computed Vref in millivolts for a given current (static helper).
    static uint16_t currentToVrefMv(uint16_t mA);

private:
    uint16_t _mA = 0;
};
