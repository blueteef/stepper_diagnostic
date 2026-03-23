#pragma once

#include "../IStepperDriver.h"

// =============================================================================
// A4988Driver — Implements IStepperDriver for the A4988 stepper driver.
//
// Current control: PWM on PIN_VREF_PWM through an RC filter to the Vref pin.
// Microstepping:   MS1/MS2/MS3 GPIO truth table.
// Step pulses:     blocking loop below STEP_TIMER_THRESHOLD_SPS; ISR above.
// Fault detection: reads PIN_DIAG (FAULT pin, active-low).
//
// Implemented in Phase 3.
// =============================================================================

class A4988Driver : public IStepperDriver {
public:
    A4988Driver();

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

    const char* driverName()    override { return "a4988"; }
    bool        supportsUART()  override { return false; }

private:
    uint16_t _currentMa  = 0;
    uint16_t _usteps     = 1;
    bool     _enabled    = false;
    bool     _forward    = true;
    volatile bool _running = false;

    void     _applyMicrostepPins();
    void     _stepBlocking(uint32_t steps, uint32_t stepsPerSec);
    void     _stepISR(uint32_t steps, uint32_t stepsPerSec);
};
