#include "TMC2208Driver.h"
#include "../../config.h"
#include <Arduino.h>

// Serial1 is used for single-wire half-duplex UART.
// TX and RX are tied to the same pin (PIN_UART) through a 1k series resistor.
#define TMC_SERIAL Serial1

TMC2208Driver::TMC2208Driver()
    : _driver(&TMC_SERIAL, TMC2208_RSENSE)
{}

void TMC2208Driver::begin() {
    // Start Serial1 with RX = TX = PIN_UART (half-duplex single-wire).
    TMC_SERIAL.begin(TMC_UART_BAUD, SERIAL_8N1, PIN_UART, PIN_UART);
    delay(100);  // allow UART to settle

    _driver.begin();

    // Enable UART control (disables PDN_UART power-down function).
    _driver.pdn_disable(true);

    // Use UART to set microstep resolution instead of MS1/MS2 pins.
    _driver.mstep_reg_select(true);

    // Default operating mode: StealthChop with autoscale.
    _driver.en_spreadCycle(false);
    _driver.pwm_autoscale(true);

    // Apply defaults.
    _driver.microsteps(_usteps);
    _driver.rms_current(_currentMa);

    // Set up GPIO.
    digitalWrite(PIN_ENABLE, HIGH);  // disabled until enable(true) called
    digitalWrite(PIN_DIR,    _forward ? HIGH : LOW);
    digitalWrite(PIN_STEP,   LOW);

    // MS1/MS2/MS3 not used for addressing on TMC2208 — drive LOW.
    digitalWrite(PIN_MS1, LOW);
    digitalWrite(PIN_MS2, LOW);
    digitalWrite(PIN_MS3, LOW);
}

void TMC2208Driver::enable(bool on) {
    _enabled = on;
    digitalWrite(PIN_ENABLE, on ? LOW : HIGH);
}

void TMC2208Driver::setDirection(bool forward) {
    _forward = forward;
    digitalWrite(PIN_DIR, forward ? HIGH : LOW);
}

void TMC2208Driver::setMicrosteps(uint16_t usteps) {
    _usteps = usteps;
    _driver.microsteps(usteps);
}

void TMC2208Driver::step(uint32_t steps, uint32_t stepsPerSec) {
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

void TMC2208Driver::stop() {
    _running = false;
}

void TMC2208Driver::setCurrentMilliamps(uint16_t mA) {
    _currentMa = mA;
    _driver.rms_current(mA);
}

uint16_t TMC2208Driver::getCurrentMilliamps() {
    return _currentMa;
}

void TMC2208Driver::setSpreadCycle(bool spreadCycle) {
    _spreadCycle = spreadCycle;
    _driver.en_spreadCycle(spreadCycle);
}

bool TMC2208Driver::isFaultActive() {
    // Check GSTAT for any fault flags via UART.
    // drv_err is set on overtemp or short-to-ground.
    uint8_t gstat = _driver.GSTAT();
    return (gstat & 0x02);  // bit 1 = drv_err
}

String TMC2208Driver::getStatusReport() {
    uint32_t drvStatus = _driver.DRV_STATUS();
    uint8_t  gstat     = _driver.GSTAT();

    String s;
    s += "driver   : tmc2208\n";
    s += "enabled  : "; s += (_enabled ? "yes" : "no"); s += "\n";
    s += "dir      : "; s += (_forward ? "forward" : "reverse"); s += "\n";
    s += "usteps   : 1/"; s += _usteps; s += "\n";
    s += "current  : "; s += _currentMa; s += " mA RMS\n";
    s += "mode     : "; s += (_spreadCycle ? "SpreadCycle" : "StealthChop"); s += "\n";

    // GSTAT flags
    s += "gstat    : ";
    if (gstat & 0x01) s += "RESET ";
    if (gstat & 0x02) s += "DRV_ERR ";
    if (gstat & 0x04) s += "UV_CP ";
    if (gstat == 0)   s += "ok";
    s += "\n";

    // DRV_STATUS flags
    s += "drv_stat : ";
    if (drvStatus & (1UL << 26)) s += "OT ";       // overtemperature shutdown
    if (drvStatus & (1UL << 25)) s += "OTPW ";     // overtemperature warning
    if (drvStatus & (1UL << 12)) s += "S2GA ";     // short coil A
    if (drvStatus & (1UL << 13)) s += "S2GB ";     // short coil B
    if (drvStatus & (1UL <<  6)) s += "OLA ";      // open load A
    if (drvStatus & (1UL <<  7)) s += "OLB ";      // open load B
    if (drvStatus & (1UL << 31)) s += "STST ";     // standstill
    uint8_t csActual = (drvStatus >> 16) & 0x1F;
    s += "\ncs_actual: "; s += csActual;

    return s;
}
