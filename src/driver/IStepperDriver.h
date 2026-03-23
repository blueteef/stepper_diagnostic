#pragma once

#include <Arduino.h>

// =============================================================================
// IStepperDriver — Pure abstract interface for all stepper driver backends.
//
// To add a new driver: create a subclass that implements every method below.
// No other files need to change.
// =============================================================================

class IStepperDriver {
public:
    virtual ~IStepperDriver() = default;

    // Lifecycle -----------------------------------------------------------------
    virtual void begin()            = 0;
    virtual void enable(bool on)    = 0;

    // Motion --------------------------------------------------------------------
    virtual void setDirection(bool forward)                     = 0;
    virtual void setMicrosteps(uint16_t usteps)                 = 0;
    virtual void step(uint32_t steps, uint32_t stepsPerSec)     = 0;
    virtual void stop()                                         = 0;

    // Current -------------------------------------------------------------------
    virtual void     setCurrentMilliamps(uint16_t mA)   = 0;
    virtual uint16_t getCurrentMilliamps()              = 0;

    // Diagnostics ---------------------------------------------------------------
    virtual bool    isFaultActive()     = 0;
    virtual String  getStatusReport()   = 0;  // human-readable

    // Optional UART-driver features — default no-op so A4988 needs no changes.
    virtual void setSpreadCycle(bool /*spreadCycle*/) {}  // false = StealthChop

    // Identity ------------------------------------------------------------------
    virtual const char* driverName()    = 0;
    virtual bool        supportsUART()  = 0;
};
