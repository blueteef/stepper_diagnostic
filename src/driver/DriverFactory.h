#pragma once

#include "IStepperDriver.h"

// =============================================================================
// DriverFactory — instantiates the correct IStepperDriver at runtime.
//
// Usage:
//   IStepperDriver* drv = DriverFactory::create("a4988");
//   if (!drv) { /* unknown driver name */ }
//
// Implemented in Phase 3 (A4988) and extended in Phase 4 (TMC2209).
// =============================================================================

class DriverFactory {
public:
    // Returns a heap-allocated driver for the given name ("a4988", "tmc2209").
    // Returns nullptr if the name is unrecognised.
    // Caller takes ownership and must delete when done.
    static IStepperDriver* create(const char* name);

    // Returns a null-terminated list of supported driver names for help text.
    static const char* const* supportedDrivers();
};
