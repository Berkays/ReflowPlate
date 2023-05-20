#ifndef __CONFIG_TEMPERATURE_H
#define __CONFIG_TEMPERATURE_H

// Options
#define NTC_STEINHART 0x0001
#define NTC_CURVE 0x0002

// Configurations
//===========================================================================
//============================= NTC CURVE ===================================
//===========================================================================
#define THERMISTOR_SERIES_RESISTOR 7500.0 // Ω
#define THERMISTOR_NUM_SAMPLES 5.0

#define THERMISTOR_CURVE(R) (449.1411 - (0.106923 * (log(R) * log(R) * log(R))) + (4.493806 * (log(R) * log(R))) - (78.36984 * log(R)))

//===========================================================================
//============================= NTC STEINHART ===============================
//===========================================================================
#define THERMISTOR_SERIES_RESISTOR 7500.0 // Ωs
#define THERMISTOR_NOMINAL_RESISTOR 10000 // Ω
#define THERMISTOR_TEMPERATURE_NOMINAL 25 // C°
#define THERMISTOR_BETA_COEFFICIENT 3950
#endif