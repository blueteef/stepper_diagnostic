#pragma once

#include "../IStepperDriver.h"
#include <TMCStepper.h>

// =============================================================================
// TMC2209Driver — IStepperDriver implementation for the TMC2209 (v1.3+).
//
// UART:  single-wire half-duplex on PIN_UART (1k series resistor on PCB).
//        Serial1 RX and TX are both set to PIN_UART.
// Current: IRUN / IHOLD via UART.
// Microstep: CHOPCONF.mres via UART.
// Mode: StealthChop (default) or SpreadCycle.
// StallGuard: registers accessible via UART; DIAG pin not wired on this PCB
//             so stall interrupt is unavailable — threshold/result via UART only.
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

    void setSpreadCycle(bool spreadCycle)       override;

    const char* driverName()    override { return "tmc2209"; }
    bool        supportsUART()  override { return true; }

    // TMC2209-specific extras (DIAG not wired — no interrupt, UART read only)
    void     setStallGuardThreshold(uint8_t sgt);
    uint16_t getStallGuardResult();

private:
    TMC2209Stepper  _driver;
    uint16_t        _currentMa   = 600;
    uint16_t        _usteps      = 16;
    bool            _enabled     = false;
    bool            _forward     = true;
    bool            _spreadCycle = false;
    volatile bool   _running     = false;
};
