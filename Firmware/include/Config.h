#include "Base.h"

#include "Features/config_temperature.h"

//===========================================================================
//============================= System Configs ==============================
//===========================================================================
#define TASKER_MAX_TASKS 15
#define BUTTON_MAX_COUNT 1
#define ADC_RESOLUTION 12
#define ADC_MAX ((2 << (ADC_RESOLUTION - 1)) - 1)
#define ADC_CORRECTION 1.0475
#define OPERATING_VOLTAGE 3.3                     // V
#define VDIV_R1 390.0                             // Ω
#define VDIV_R2 100.0                             // Ω
#define INPUT_VOLTAGE 12.0                        // V
#define MIN_ALLOWED_VOLTAGE (INPUT_VOLTAGE - 1.0) // V
#define MAX_ALLOWED_VOLTAGE (INPUT_VOLTAGE + 1.0) // V

#define INVERT_SCREEN DISABLED

#define REFLOW_DURATION 300  // s. Profile is divided into RESOLUTION sized sections.
#define REFLOW_RESOLUTION 15 // s.
#define REFLOW_POINT_COUNT (REFLOW_DURATION / REFLOW_RESOLUTION)
#define AUTOMATIC_MODE_HISTORY_RESOLUTION 4

#define TEMP_UPDATE_INTERVAL 250  // ms
#define HEAT_UPDATE_INTERVAL 1000 // ms

#define INVERT_ENCODER DISABLED
#define ENCODER_THRESHOLD 50                 // ±
#define THERMAL_RUNAWAY_THRESHOLD 4          // °C
#define THERMAL_RUNAWAY_TIMESPAN (20 * 1000) // ms
#define MAX_SAFE_TEMP 70                     // °C

//===========================================================================
//============================= Features ====================================
//===========================================================================
#define SERIAL_COMM ENABLED
#define WAIT_SAFE_TEMP ENABLED
#define FAN_COOLDOWN DISABLED
#define PREHEAT ENABLED

#define TEMPERATURE_METHOD NTC_CURVE

//===========================================================================
//============================= Heater Settings =============================
//===========================================================================
#define HEATING_ELEMENT_NOMINAL_RESISTANCE 1.5       // Ω
#define HEATING_ELEMENT_RESISTANCE_PER_DEGREE 0.0085 // °C/Ω
#define MAX_CURRENT 5.5                              // A
#define MIN_CURRENT 1.0                              // A
#define MAX_TEMP 160                                 // °C
#define MIN_TEMP 30                                  // °C
#define KI 0.1
#define KP 8
#define KD 0