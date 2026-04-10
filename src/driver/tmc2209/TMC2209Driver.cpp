#include "TMC2209Driver.h"
#include "../../config.h"
#include <Arduino.h>

#define TMC_SERIAL  Serial1
#define TMC_ADDR    0   // MS1=LOW, MS2=LOW => address 0

TMC2209Driver::TMC2209Driver()
    : _driver(&TMC_SERIAL, TMC2209_RSENSE, TMC_ADDR)
{}

void TMC2209Driver::begin() {
    // Single-wire half-duplex: RX and TX on same pin.
    TMC_SERIAL.begin(TMC_UART_BAUD, SERIAL_8N1, PIN_UART, PIN_UART);
    delay(100);

    _driver.begin();

    // Enable UART control (disables PDN_UART power-down function).
    _driver.pdn_disable(true);

    // Use UART to set microstep resolution instead of MS1/MS2 pins.
    _driver.mstep_reg_select(true);

    // Default: StealthChop with autoscale.
    _driver.en_spreadCycle(false);
    _driver.pwm_autoscale(true);

    // Apply defaults.
    _driver.microsteps(_usteps);
    _driver.rms_current(_currentMa);

    // GPIO defaults.
    digitalWrite(PIN_ENABLE, HIGH);  // disabled until enable(true)
    digitalWrite(PIN_DIR,    _forward ? HIGH : LOW);
    digitalWrite(PIN_STEP,   LOW);

    // MS1/MS2 LOW = UART address 0; MS3 unused on TMC2209.
    digitalWrite(PIN_MS1, LOW);
    digitalWrite(PIN_MS2, LOW);
    digitalWrite(PIN_MS3, LOW);
}

void TMC2209Driver::enable(bool on) {
    _enabled = on;
    digitalWrite(PIN_ENABLE, on ? LOW : HIGH);
}

void TMC2209Driver::setDirection(bool forward) {
    _forward = forward;
    digitalWrite(PIN_DIR, forward ? HIGH : LOW);
}

void TMC2209Driver::setMicrosteps(uint16_t usteps) {
    _usteps = usteps;
    _driver.microsteps(usteps);
}

void TMC2209Driver::step(uint32_t steps, uint32_t stepsPerSec) {
    if (stepsPerSec == 0 || steps == 0) return;

    _running = true;

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

void TMC2209Driver::stop() {
    _running = false;
}

void TMC2209Driver::setCurrentMilliamps(uint16_t mA) {
    _currentMa = mA;
    _driver.rms_current(mA);
}

uint16_t TMC2209Driver::getCurrentMilliamps() {
    return _currentMa;
}

void TMC2209Driver::setSpreadCycle(bool spreadCycle) {
    _spreadCycle = spreadCycle;
    _driver.en_spreadCycle(spreadCycle);
}

void TMC2209Driver::setStallGuardThreshold(uint8_t sgt) {
    // DIAG pin not wired — stall interrupt unavailable.
    // Write SGTHRS so the value is set if DIAG is ever connected.
    _driver.SGTHRS(sgt);
}

uint16_t TMC2209Driver::getStallGuardResult() {
    return _driver.SG_RESULT();
}

bool TMC2209Driver::isFaultActive() {
    uint8_t gstat = _driver.GSTAT();
    return (gstat & 0x02);  // bit 1 = drv_err
}

String TMC2209Driver::getStatusReport() {
    uint32_t drvStatus = _driver.DRV_STATUS();
    uint8_t  gstat     = _driver.GSTAT();
    uint16_t sgResult  = _driver.SG_RESULT();

    String s;
    s += "driver   : tmc2209\n";
    s += "enabled  : "; s += (_enabled ? "yes" : "no"); s += "\n";
    s += "dir      : "; s += (_forward ? "forward" : "reverse"); s += "\n";
    s += "usteps   : 1/"; s += _usteps; s += "\n";
    s += "current  : "; s += _currentMa; s += " mA RMS\n";
    s += "mode     : "; s += (_spreadCycle ? "SpreadCycle" : "StealthChop"); s += "\n";
    s += "sg_result: "; s += sgResult; s += " (DIAG not wired)\n";

    s += "gstat    : ";
    if (gstat & 0x01) s += "RESET ";
    if (gstat & 0x02) s += "DRV_ERR ";
    if (gstat & 0x04) s += "UV_CP ";
    if (gstat == 0)   s += "ok";
    s += "\n";

    s += "drv_stat : ";
    if (drvStatus & (1UL << 26)) s += "OT ";
    if (drvStatus & (1UL << 25)) s += "OTPW ";
    if (drvStatus & (1UL << 12)) s += "S2GA ";
    if (drvStatus & (1UL << 13)) s += "S2GB ";
    if (drvStatus & (1UL <<  6)) s += "OLA ";
    if (drvStatus & (1UL <<  7)) s += "OLB ";
    if (drvStatus & (1UL << 31)) s += "STST ";
    uint8_t csActual = (drvStatus >> 16) & 0x1F;
    s += "\ncs_actual: "; s += csActual;

    return s;
}
