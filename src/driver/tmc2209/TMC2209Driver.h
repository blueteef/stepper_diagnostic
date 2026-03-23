#pragma once

#include "../IStepperDriver.h"
#include <TMCStepper.h>

// =============================================================================
// TMC2209Driver — Implements IStepperDriver for the TMC2209 stepper driver.
//
// UART:  single-wire half-duplex on PIN_UART (1k series resistor on PCB).
//        Serial1 is used with RX == TX == PIN_UART.
// Current: IRUN / IHOLD via UART (driver.rms_current()).
// Microstep: CHOPCONF.mres via UART.
// Diagnostics: DRV_STATUS, SG_RESULT, TSTEP, GSTAT via UART + PIN_DIAG.
//
// Implemented in Phase 4.
// =============================================================================

class TMC2209Driver : public IStepperDriver {
public:
    TMC2209Driver();

    void begin()            override;
    void enable(bool on)    override;

    void setDirection(bool forward)                     override;
    void setMicrosteps(uint16_t usteps)                 override;
    void step(uint32_t steps, uint32_t stepsPerSec)     override;
    void stop()                                         override;

    void     setCurrentMilliamps(uint16_t mA)   override;
    uint16_t getCurrentMilliamps()              override;

    bool    isFaultActive()     override;
    String  getStatusReport()   override;

    const char* driverName()    override { return "tmc2209"; }
    bool        supportsUART()  override { return true; }

    // TMC2209-specific extras
    void setSpreadCycle(bool spreadCycle);  // false = StealthChop
    void setStallGuardThreshold(int8_t sgt);
    uint16_t getStallGuardResult();

private:
    TMC2209Stepper _driver;
    uint16_t       _currentMa = 0;
    uint16_t       _usteps    = 16;  // TMC2209 default
    bool           _forward   = true;
    volatile bool  _running   = false;

    void _stepBlocking(uint32_t steps, uint32_t stepsPerSec);
    void _stepISR(uint32_t steps, uint32_t stepsPerSec);
};
