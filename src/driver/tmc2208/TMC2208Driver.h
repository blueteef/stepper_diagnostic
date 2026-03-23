#pragma once

#include "../IStepperDriver.h"
#include <TMCStepper.h>

// =============================================================================
// TMC2208Driver — IStepperDriver implementation for the TMC2208.
//
// UART: single-wire half-duplex on PIN_UART (1k series resistor on PCB).
//       Serial1 RX and TX are both set to PIN_UART.
// Current: IRUN / IHOLD via UART.
// Microstep: CHOPCONF.mres via UART.
// Mode: StealthChop (default) or SpreadCycle via en_spreadCycle().
// Diagnostics: GSTAT + DRV_STATUS via UART.
//              No StallGuard on TMC2208 (use TMC2209 for that).
// =============================================================================

class TMC2208Driver : public IStepperDriver {
public:
    TMC2208Driver();

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

    void setSpreadCycle(bool spreadCycle) override;

    const char* driverName()    override { return "tmc2208"; }
    bool        supportsUART()  override { return true; }

private:
    TMC2208Stepper  _driver;
    uint16_t        _currentMa   = 600;   // default 600 mA RMS
    uint16_t        _usteps      = 16;
    bool            _enabled     = false;
    bool            _forward     = true;
    bool            _spreadCycle = false;  // default StealthChop
    volatile bool   _running     = false;
};
