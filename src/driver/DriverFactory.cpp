#include "DriverFactory.h"
#include "a4988/A4988Driver.h"
#include "tmc2208/TMC2208Driver.h"

#include <string.h>

IStepperDriver* DriverFactory::create(const char* name) {
    if (strcasecmp(name, "a4988")   == 0) return new A4988Driver();
    if (strcasecmp(name, "tmc2208") == 0) return new TMC2208Driver();
    return nullptr;
}

static const char* const SUPPORTED[] = { "a4988", "tmc2208", nullptr };

const char* const* DriverFactory::supportedDrivers() {
    return SUPPORTED;
}
