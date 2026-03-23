#pragma once

// =============================================================================
// config.h — Single source of truth for all pin assignments and compile-time
//             defaults. Change this file when revising hardware.
// =============================================================================

// -----------------------------------------------------------------------------
// GPIO Pin Assignments (ESP32-C3 Supermini)
// -----------------------------------------------------------------------------

#define PIN_STEP        0   // OUT  — Step pulse
#define PIN_DIR         1   // OUT  — Direction (HIGH = forward)
#define PIN_ENABLE      2   // OUT  — Driver enable, active-low

#define PIN_MS1         3   // OUT  — Microstep bit 0 / TMC2209 UART addr bit 0
#define PIN_MS2         4   // OUT  — Microstep bit 1 / TMC2209 UART addr bit 1
#define PIN_MS3         5   // OUT  — Microstep bit 2 (A4988 only; NC on TMC2209)
#define PIN_SPREAD      6   // OUT  — SpreadCycle/StealthChop select (TMC2209 CFG)

#define PIN_DIAG        7   // IN   — DIAG / FAULT (open-drain, pulled up)
#define PIN_INDEX       8   // IN   — TMC2209 INDEX / unused on A4988 (pulled up)

#define PIN_UART        9   // BIDIR — Half-duplex single-wire UART to TMC2209
                            //         1k series resistor required on PCB

#define PIN_VREF_PWM   10   // OUT  — PWM -> 10k+100nF RC filter -> A4988 Vref

// GPIO 18/19: USB D-/D+, never touch.
// GPIO 20/21: spare, reserved for future use.

// -----------------------------------------------------------------------------
// Step Pulse Timing
// -----------------------------------------------------------------------------

// Pulse width for STEP pin (microseconds). Minimum for A4988 is 1us; use 2us
// for margin.
#define STEP_PULSE_US       2

// Below this threshold, step() uses a blocking delayMicroseconds() loop.
// Above it, an esp_timer ISR is used to keep the CLI responsive.
#define STEP_TIMER_THRESHOLD_SPS  2000

// -----------------------------------------------------------------------------
// A4988 Current Control (Vref via PWM)
// -----------------------------------------------------------------------------

// Sense resistor value on the driver board (ohms). Common carrier boards use
// 0.1 ohm (100 mohm). Adjust if using a different board.
#define A4988_RSENSE_OHMS   0.1f

// PWM channel, frequency, and resolution for Vref output.
#define VREF_PWM_CHANNEL    0
#define VREF_PWM_FREQ_HZ    20000
#define VREF_PWM_RESOLUTION 10      // bits (0–1023)

// Vref formula:  Vref(V) = Itrip(A) * 8 * Rsense
// Duty cycle  =  (Vref / Vcc) * (2^resolution - 1)
// Vcc on ESP32-C3 GPIO is 3.3V.
#define VREF_VCC            3.3f

// -----------------------------------------------------------------------------
// TMC2208 UART
// -----------------------------------------------------------------------------

#define TMC_UART_BAUD       115200

// Sense resistor on the TMC2208 carrier board (ohms).
// Most common boards (BTT, Fysetc, MKS) use 0.11 ohm.
#define TMC2208_RSENSE      0.11f

// -----------------------------------------------------------------------------
// Serial CLI
// -----------------------------------------------------------------------------

#define CLI_BAUD            115200
#define CLI_LINE_BUF_SIZE   128     // max input line length in bytes
