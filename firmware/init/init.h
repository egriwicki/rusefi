/**
 * @file init.h
 */

#pragma once

class Logging;

// Call this once at startup to initialize, configure, and subscribe sensors
void initNewSensors(Logging* logger);

// Call this whenever the configuration may have changed, so any sensors
// can be reconfigured with the new settings.
// Note: this may not be necessarily possible for all sensors, so some may
// do nothing when this is called.
void reconfigureSensors();
